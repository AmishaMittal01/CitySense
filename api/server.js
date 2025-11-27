const express = require("express");
const cors = require("cors");
const { execFile } = require("child_process");
const path = require("path");

const app = express();
app.use(cors());

const exePath =
  process.platform === "win32"
    ? path.join(__dirname, "..", "city.exe")
    : path.join(__dirname, "..", "city");
// ---- Dynamic simulation state ----
let parkingState = null; 
let trafficNoise = 0;

const PHASES = [
  { code: "NS_GREEN",  text: "N-S green, E-W red",  allowGo: true },
  { code: "NS_YELLOW", text: "N-S yellow, E-W red", allowGo: true },
  { code: "EW_GREEN",  text: "N-S red, E-W green",  allowGo: false },
  { code: "EW_YELLOW", text: "N-S red, E-W yellow", allowGo: false },
];
// Our real-world intersections mapped to node IDs
let intersectionsState = [
  { id: "fortis",    name: "Fortis Crossing",            node: 2,  phaseIndex: 0 }, // Fortis Hospital
  { id: "corenthum", name: "Corenthum Junction",         node: 3,  phaseIndex: 1 }, // The Corenthum
  { id: "nh9",       name: "NH-9 Loop",                  node: 12, phaseIndex: 2 }, // NH-9 ramp
  { id: "shipra",    name: "Shipra–IHC Cross",           node: 9,  phaseIndex: 0 }, // IHC / Shipra area
];
function runCity(args) {
  return new Promise((resolve, reject) => {
    execFile(exePath, args, { encoding: "utf8" }, (err, stdout, stderr) => {
      if (err) {
        console.error("Engine error:", err, stderr);
        return reject(err);
      }
      resolve(stdout.trim());
    });
  });
}
// /route?src=7&dest=8
app.get("/route", async (req, res) => {
  try {
    const src = parseInt(req.query.src, 10) || 1;
    const dest = parseInt(req.query.dest, 10) || src;

    // 1) distances from src to all nodes
    const outDist = await runCity(["route", String(src)]);
    const parts = outDist.split(/\s+/).map((x) => x.trim());
    const n = parseInt(parts[0], 10) || 0;
    const distances = [];
    for (let i = 0; i < n; i++) {
      const nodeId = i + 1;
      const token = parts[1 + i];
      let distance = Infinity;
      if (token && token !== "inf") {
        distance = parseFloat(token);
      }
      distances.push({ node: nodeId, distance });
    }
    // 2) path from src to dest
    const outPath = await runCity([
      "route-path",
      String(src),
      String(dest),
    ]);
    const pt = outPath.split(/\s+/).map((x) => x.trim());
    const k = parseInt(pt[0], 10) || 0;
    const path =
      k > 0 ? pt.slice(1, 1 + k).map((v) => parseInt(v, 10)) : [];

    res.json({ src, dest, distances, path });
  } catch (e) {
    console.error(e);
    res.status(500).json({ error: "Route computation failed" });
  }
});
// /emergency-route?src=7
app.get("/emergency-route", async (req, res) => {
  try {
    const src = parseInt(req.query.src, 10) || 1;
    const out = await runCity(["emergency-route", String(src)]);
    const parts = out.split(/\s+/).map((x) => x.trim());
    const n = parseInt(parts[0], 10) || 0;
    const distances = [];
    for (let i = 0; i < n; i++) {
      const nodeId = i + 1;
      const token = parts[1 + i];
      let distance = Infinity;
      if (token && token !== "inf") {
        distance = parseFloat(token);
      }
      distances.push({ node: nodeId, distance });
    }
    res.json({ src, distances });
  } catch (e) {
    console.error(e);
    res.status(500).json({ error: "Emergency route failed" });
  }
});
// /congestion?start=0&end=2
app.get("/congestion", async (req, res) => {
  try {
    const start = parseInt(req.query.start, 10) || 0;
    const end = parseInt(req.query.end, 10) || 0;

    // Get base value from C++ (segment tree)
    const out = await runCity([
      "congestion",
      String(start),
      String(end),
    ]);
    let baseVal = parseInt(out.trim(), 10);
    if (!Number.isFinite(baseVal)) baseVal = 0;

    // Add small dynamic noise: -3..+3, clamped
    const delta = Math.floor(Math.random() * 7) - 3;
    trafficNoise += delta;
    if (trafficNoise > 10) trafficNoise = 10;
    if (trafficNoise < -10) trafficNoise = -10;

    let value = baseVal + trafficNoise;
    if (value < 0) value = 0;

    res.json({ start, end, value });
  } catch (e) {
    console.error(e);
    res.status(500).json({ error: "Congestion query failed" });
  }
});

// /parking-status
app.get("/parking-status", async (req, res) => {
  try {
    // Initialize from C++ once (for total capacities)
    if (!parkingState) {
      const out = await runCity(["parking-status"]);
      // format: "zone1:3/10,zone2:5/8,zone3:7/12"
      const zones = out
        .split(",")
        .map((z) => z.trim())
        .filter(Boolean)
        .map((z) => {
          const [name, rest] = z.split(":");
          const [used, total] = rest.split("/").map((x) => parseInt(x, 10));
          return { name, used, total };
        });
      parkingState = zones;
    }

    // On every call, simulate cars parking / leaving
    parkingState = parkingState.map((z) => {
      // random change: -1, 0, or +1
      const delta = Math.floor(Math.random() * 3) - 1;
      let used = z.used + delta;
      if (used < 0) used = 0;
      if (used > z.total) used = z.total;
      return { ...z, used };
    });

    res.json({ zones: parkingState });
  } catch (e) {
    console.error(e);
    res.status(500).json({ error: "Parking status failed" });
  }
});

// /signal-status — multiple intersections, independent cycles
app.get("/signal-status", async (req, res) => {
  try {
    // advance each intersection's phase
    intersectionsState = intersectionsState.map((s) => {
      const nextIndex = (s.phaseIndex + 1) % PHASES.length;
      return { ...s, phaseIndex: nextIndex };
    });

    // build API payload
    const intersections = intersectionsState.map((s) => {
      const phase = PHASES[s.phaseIndex];
      return {
        id: s.id,
        name: s.name,
        node: s.node,          // matches Graph node ID
        phaseCode: phase.code, // e.g. "NS_GREEN"
        phaseText: phase.text, // e.g. "N-S green, E-W red"
        allowGo: phase.allowGo // true => vehicles allowed to go
      };
    });

    // keep backward-compatible simple status text (for your sidebar)
    const first = intersections[0];
    const status = `${first.name}: ${first.phaseText}`;

    res.json({ status, intersections });
  } catch (e) {
    console.error(e);
    res.status(500).json({ error: "Signal status failed" });
  }
});


const PORT = 5000;
app.listen(PORT, () => {
  console.log(`API running on http://localhost:${PORT}`);
});
