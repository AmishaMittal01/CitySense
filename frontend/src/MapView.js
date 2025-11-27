import React, { useMemo, useState, useEffect } from "react";
import {
  MapContainer,
  TileLayer,
  CircleMarker,
  Tooltip,
  Polyline,
} from "react-leaflet";

// ---- LANDMARK NODES ----
const NODES = [
  { id: 1,  name: "Sector 62 Metro", lat: 28.616986, lng: 77.373589 },
  { id: 2,  name: "Fortis Hospital", lat: 28.618888, lng: 77.372459 },
  { id: 3,  name: "The Corenthum", lat: 28.628151, lng: 77.367783 },
  { id: 4,  name: "Peace Softtech C-56/21", lat: 28.621033, lng: 77.362389 },
  { id: 5,  name: "Logix Cyber Park", lat: 28.6245, lng: 77.3685 },
  { id: 6,  name: "Stellar IT Park", lat: 28.6270, lng: 77.3715 },
  { id: 7,  name: "JIIT Sector 62", lat: 28.6298, lng: 77.3679 },
  { id: 8,  name: "Shipra Mall", lat: 28.6415, lng: 77.3714 },
  { id: 9,  name: "Indirapuram Habitat Centre", lat: 28.6425, lng: 77.3730 },
  { id: 10, name: "Aditya Mall", lat: 28.6435, lng: 77.3770 },
  { id: 11, name: "Orange County", lat: 28.6405, lng: 77.3685 },
  { id: 12, name: "NH-9 Ramp / Kala Patthar", lat: 28.6390, lng: 77.3805 },
];

// ---- ROADS ----
const ROADS = [
  { from: 1, to: 2 }, { from: 2, to: 6 }, { from: 6, to: 3 }, { from: 3, to: 5 },
  { from: 5, to: 4 }, { from: 4, to: 1 }, { from: 7, to: 2 }, { from: 7, to: 3 },
  { from: 7, to: 5 }, { from: 2, to: 12 }, { from: 6, to: 12 }, { from: 12, to: 8 },
  { from: 8, to: 9 }, { from: 9, to: 10 }, { from: 10, to: 12 }, { from: 9, to: 11 },
  { from: 11, to: 8 }
];

// ---- PARKING ----
const PARKING_GEOMETRY = [
  { name: "zone1", label: "JIIT / Fortis Parking", lat: 28.6195, lng: 77.3720 },
  { name: "zone2", label: "IT Belt Parking", lat: 28.6265, lng: 77.3695 },
  { name: "zone3", label: "Indirapuram Parking", lat: 28.6420, lng: 77.3720 },
];

export default function MapView({
  distances,
  parking,
  emergencyPath,
  routePath,
  signal,
  onSelectNode,
}) {
  const center = [28.6335, 77.3720];

  // ---- Node lookup map ----
  const nodeById = useMemo(() => {
    const m = {};
    NODES.forEach((n) => (m[n.id] = n));
    return m;
  }, []);

  // ---- Heatmap colors ----
  const nodesWithDist = useMemo(() => {
    const dMap = {};
    distances.forEach(d => dMap[d.node] = d.distance);

    return NODES.map(n => ({
      ...n,
      distance: dMap[n.id] ?? Infinity
    }));
  }, [distances]);

  const maxDist = Math.max(...nodesWithDist.map(n => Number.isFinite(n.distance) ? n.distance : 0), 1);

  const getColor = d => {
    if (!Number.isFinite(d)) return "#999";
    const t = d / maxDist;
    if (t < 0.33) return "#16a34a"; 
    if (t < 0.66) return "#eab308";
    return "#dc2626";
  };

  // ---- ROUTE COORDINATES ----
  const routeCoords = useMemo(() => {
    if (!routePath) return [];
    return routePath.map(id => [nodeById[id].lat, nodeById[id].lng]);
  }, [routePath]);

  const [routeProgress, setRouteProgress] = useState(0);

  useEffect(() => {
    if (routeCoords.length < 2) return;

    setRouteProgress(0);

    const int = setInterval(() => {
      setRouteProgress(p => {
        const next = p + 0.02;
        if (next >= routeCoords.length - 1) return routeCoords.length - 1;
        return next;
      });
    }, 100);

    return () => clearInterval(int);
  }, [routeCoords]);

  const routeMarkerPos = useMemo(() => {
    if (routeCoords.length < 2) return null;

    const i = Math.floor(routeProgress);
    const t = routeProgress - i;

    const A = routeCoords[i];
    const B = routeCoords[i + 1] ?? A;

    return [A[0] + (B[0] - A[0]) * t, A[1] + (B[1] - A[1]) * t];
  }, [routeProgress, routeCoords]);

  // ---- EMERGENCY ----
  const emergencyCoords = useMemo(() => {
    if (!emergencyPath) return [];
    return emergencyPath.map(id => [nodeById[id].lat, nodeById[id].lng]);
  }, [emergencyPath]);

  const [emIndex, setEmIndex] = useState(0);

  useEffect(() => {
    if (emergencyCoords.length < 2) return;

    setEmIndex(0);
    const int = setInterval(() => {
      setEmIndex(i => Math.min(i + 1, emergencyCoords.length - 1));
    }, 500);

    return () => clearInterval(int);
  }, [emergencyCoords]);

  const emergencyMarkerPos = emergencyCoords[emIndex] ?? null;

  // ---- INTERSECTIONS (no PNGs, simple colored dots) ----
  const intersectionMarkers = useMemo(() => {
    if (!signal || !signal.intersections) return [];

    return signal.intersections.map(s => {
      const node = nodeById[s.node];
      return {
        id: s.id,
        lat: node.lat,
        lng: node.lng,
        color: s.allowGo ? "#16a34a" : "#dc2626",
        text: s.phaseText
      };
    });
  }, [signal, nodeById]);

  // ---- PARKING ----
  const parkingMarkers = useMemo(() => {
    if (!parking) return [];
    return parking.zones.map(z => {
      const geo = PARKING_GEOMETRY.find(g => g.name === z.name);
      if (!geo) return null;

      const ratio = z.used / z.total;
      let color = "#16a34a";
      if (ratio > 0.7) color = "#dc2626";
      else if (ratio > 0.4) color = "#eab308";

      return { ...geo, used: z.used, total: z.total, color };
    }).filter(Boolean);
  }, [parking]);

  return (
    <div className="map-wrapper">
      <MapContainer center={center} zoom={14} style={{ height: "400px", width: "100%" }}>
        <TileLayer
          attribution="&copy; CartoDB"
          url="https://cartodb-basemaps-a.global.ssl.fastly.net/light_all/{z}/{x}/{y}.png"
        />

        {/* Roads */}
        {ROADS.map((r, i) => {
          const A = nodeById[r.from];
          const B = nodeById[r.to];
          return (
            <Polyline key={i} positions={[[A.lat, A.lng],[B.lat,B.lng]]} pathOptions={{color:"#555"}} />
          );
        })}

        {/* Nodes */}
        {nodesWithDist.map(n => (
          <CircleMarker
            key={n.id}
            center={[n.lat, n.lng]}
            radius={8}
            pathOptions={{ fillColor: getColor(n.distance), color: getColor(n.distance), fillOpacity: 0.9 }}
            eventHandlers={{ click: () => onSelectNode?.(n) }}
          >
            <Tooltip>
              {n.name}<br/>Distance: {Number.isFinite(n.distance) ? n.distance.toFixed(2) : "∞"}
            </Tooltip>
          </CircleMarker>
        ))}

        {/* Parking */}
        {parkingMarkers.map(p => (
          <CircleMarker
            key={p.name}
            center={[p.lat, p.lng]}
            radius={12}
            pathOptions={{ color: p.color, fillColor: p.color, fillOpacity: 0.5 }}
          >
            <Tooltip>{p.label}<br/>{p.used}/{p.total}</Tooltip>
          </CircleMarker>
        ))}

        {/* Signal markers */}
        {intersectionMarkers.map(s => (
          <CircleMarker
            key={s.id}
            center={[s.lat, s.lng]}
            radius={10}
            pathOptions={{ fillColor: s.color, color: s.color, fillOpacity: 1 }}
          >
            <Tooltip>{s.text}</Tooltip>
          </CircleMarker>
        ))}

        {/* Route polyline */}
        {routeCoords.length > 0 && (
          <Polyline positions={routeCoords} pathOptions={{ color: "#38bdf8" }} />
        )}

        {/* Route vehicle */}
        {routeMarkerPos && (
          <CircleMarker
            center={routeMarkerPos}
            radius={7}
            pathOptions={{ color: "#7c3aed", fillColor: "#8b5cf6", fillOpacity: 1 }}
          >
            <Tooltip>Route Vehicle</Tooltip>
          </CircleMarker>
        )}

        {/* Emergency polyline */}
        {emergencyCoords.length > 0 && (
          <Polyline positions={emergencyCoords} pathOptions={{ color: "#ef4444" }} />
        )}

        {/* Emergency vehicle */}
        {emergencyMarkerPos && (
          <CircleMarker
            center={emergencyMarkerPos}
            radius={7}
            pathOptions={{ color: "#f97316", fillColor: "#fb923c", fillOpacity: 1 }}
          >
            <Tooltip>Emergency Vehicle</Tooltip>
          </CircleMarker>
        )}
      </MapContainer>
    </div>
  );
}
