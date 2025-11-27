import React, { useState } from "react";
import api from "./api";
import MapView from "./MapView";
import "./App.css";

// Landmark list for dropdowns
const LANDMARKS = [
  { id: 1, name: "Noida Sector 62 Metro (Transit)" },
  { id: 2, name: "Fortis Hospital (Medical)" },
  { id: 3, name: "The Corenthum (IT Park)" },
  { id: 4, name: "Peace Softtech (Corporate)" },
  { id: 5, name: "Logix Cyber Park (IT Park)" },
  { id: 6, name: "Stellar IT Park (IT Park)" },
  { id: 7, name: "JIIT Sector 62 (Campus)" },
  { id: 8, name: "Shipra Mall (Commercial)" },
  { id: 9, name: "Indirapuram Habitat Centre (Commercial)" },
  { id: 10, name: "Aditya Mall (Residential)" },
  { id: 11, name: "Orange County (Residential)" },
  { id: 12, name: "NH9 Ramp / Kala Patthar (Highway)" },
];

function App() {
  const [routeData, setRouteData] = useState(null);
  const [routePath, setRoutePath] = useState([]);
  const [emergencyData, setEmergencyData] = useState(null);
  const [congestion, setCongestion] = useState(null);
  const [parking, setParking] = useState(null);
  const [signal, setSignal] = useState(null);
  const [srcNode, setSrcNode] = useState(7);
  const [destNode, setDestNode] = useState(8);
  const [emergencyPath, setEmergencyPath] = useState([]);
  const [selectedNode, setSelectedNode] = useState(null);

  const fetchRoute = async () => {
    const res = await api.get("/route", {
      params: { src: srcNode, dest: destNode },
    });
    setRouteData(res.data);
    setRoutePath(res.data.path || []);
    setEmergencyData(null);
    setEmergencyPath([]);
  };

  const fetchEmergency = async () => {
    const res = await api.get("/emergency-route", { params: { src: srcNode } });
    setEmergencyData(res.data);
    setRoutePath([]);
    const finite = res.data.distances.filter((d) => Number.isFinite(d.distance));
    finite.sort((a, b) => a.distance - b.distance);
    const pathIds = finite.map((d) => d.node);
    setEmergencyPath(pathIds);
  };

  const fetchCongestion = async () => {
    const res = await api.get("/congestion", { params: { start: 0, end: 2 } });
    setCongestion(res.data);
  };

  const fetchParking = async () => {
    const res = await api.get("/parking-status");
    setParking(res.data);
  };

  const fetchSignal = async () => {
    const res = await api.get("/signal-status");
    setSignal(res.data);
  };

  const currentDistances =
    emergencyData?.distances || routeData?.distances || [];

  return (
    <div className="app">

      {/* Header */}
      <header className="header">
        <h2>CitySense</h2>
        <p>Urban Traffic Simulation</p>
      </header>

      <div className="content">
        <div className="left-panel">

          {/* Route & Emergency */}
          <div className="card">
            <h2>Route & Emergency Controls</h2>

            <label>
              Source Landmark:
              <select
                value={srcNode}
                onChange={(e) => setSrcNode(Number(e.target.value))}
              >
                {LANDMARKS.map((l) => (
                  <option key={l.id} value={l.id}>
                    {l.name}
                  </option>
                ))}
              </select>
            </label>

            <label>
              Destination Landmark:
              <select
                value={destNode}
                onChange={(e) => setDestNode(Number(e.target.value))}
              >
                {LANDMARKS.map((l) => (
                  <option key={l.id} value={l.id}>
                    {l.name}
                  </option>
                ))}
              </select>
            </label>

            <div className="button-row">
              <button onClick={fetchRoute}>Compute Route</button>
              <button onClick={fetchEmergency}>Emergency Route</button>
            </div>
          </div>

          {/* Traffic & Parking */}
          <div className="card">
            <h2>Traffic & Parking</h2>
            <div className="button-row">
              <button onClick={fetchCongestion}>Traffic Load</button>
              <button onClick={fetchParking}>Parking Status</button>
              <button onClick={fetchSignal}>Signal Status</button>
            </div>

            <div className="stats">
              {congestion && (
                <p>
                  <strong>Traffic Load:</strong> {congestion.value}
                </p>
              )}

              {parking && (
                <div>
                  <strong>Parking:</strong>
                  <ul>
                    {parking.zones.map((z) => (
                      <li key={z.name}>
                        {z.name}: {z.used}/{z.total}
                      </li>
                    ))}
                  </ul>
                </div>
              )}

              {signal && (
                <p>
                  <strong>Signal:</strong> {signal.status}</p>
              )}
            </div>
          </div>
        </div>

        {/* Right panel */}
        <div className="right-panel">
          <div className="card">
            <h2>City Map</h2>

            <MapView
              distances={currentDistances}
              parking={parking}
              emergencyPath={emergencyPath}
              routePath={routePath}
              signal={signal}
              onSelectNode={setSelectedNode}
            />
          </div>

          {/* Debug */}
          <div className="card">
            <h2>Debug & Node Details</h2>

            {selectedNode && (
              <div className="selected-node">
                <h4>Selected Node</h4>
                <p>
                  <strong>{selectedNode.name}</strong> (ID {selectedNode.id})
                </p>

                {currentDistances.length > 0 && (
                  <p>
                    Distance from source:{" "}
                    {(() => {
                      const d = currentDistances.find(
                        (x) => x.node === selectedNode.id
                      );
                      return d && Number.isFinite(d.distance)
                        ? d.distance.toFixed(2)
                        : "N/A";
                    })()}
                  </p>
                )}
              </div>
            )}

            {/* Route Data with names */}
            {routeData && (
              <>
                <h4>Route data</h4>
                <pre>
                  {JSON.stringify(
                    {
                      source: LANDMARKS.find((l) => l.id === routeData.src)?.name,
                      destination: LANDMARKS.find((l) => l.id === routeData.dest)
                        ?.name,
                      path: routePath.map(
                        (id) => LANDMARKS.find((l) => l.id === id)?.name
                      ),
                      distances: routeData.distances.map((d) => ({
                        node: LANDMARKS.find((l) => l.id === d.node)?.name,
                        distance: d.distance,
                      })),
                    },
                    null,
                    2
                  )}
                </pre>
              </>
            )}

            {emergencyData && (
              <>
                <h4>Emergency distances</h4>
                <pre>
                  {JSON.stringify(
                    emergencyData.distances.map((d) => ({
                      node: LANDMARKS.find((l) => l.id === d.node)?.name,
                      distance: d.distance,
                    })),
                    null,
                    2
                  )}
                </pre>
              </>
            )}
          </div>
        </div>
      </div>
    </div>
  );
}

export default App;
