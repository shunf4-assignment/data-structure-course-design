if (typeof(rawSHMetroData) == "string")
    rawSHMetroData = JSON.parse(rawSHMetroData)
labelDict = {"left": [-35, 0], "right": [35,0], "top": [0, -23], "bottom": [0, 23], "bottomleft": [-30, 20], "bottomright": [30, 20], "topleft": [-30, -20], "topright": [30, -20]}

newStations = {}

for (var sid in rawSHMetroData.stations){
    var old = rawSHMetroData.stations[sid]
    var ne = {}
    ne.x = old.loc.x
    ne.y = old.loc.y
    ne.name = old.name
    ne.id = old.id
    var labelPos = labelDict[old.label.split(".")[0]]
    ne.captionOffsetX = labelPos[0]
    ne.captionOffsetY = labelPos[1]
    
    newStations[sid] = ne
}

newLines = {}

for (var lid in rawSHMetroData.lines){
    var old = rawSHMetroData.lines[lid]
    var ne = {}
    ne.name = old.name
    ne.id = old.id
    ne.drawOffset = old.name == "3号线" ? -4 : old.name == "4号线" ? 4 : 0
    ne.color = old.color
    ne.stations = []
    for (var lsid in old.stations){
        if (typeof(old.stations[lsid]) == "string")
            ne.stations.push(old.stations[lsid].replace(/([btlr][btlr])/i, ""))
    }
    
    newLines[lid] = ne
}

newMapData = {stations: newStations, lines: newLines}
console.log(JSON.stringify(newMapData))