function fetchGraphs() {
  fetch(`/api/graphs?id=${document.getElementById('index').value}`)
    .then(res => res.json()).then(data => {
      const top = document.getElementById('graph');
      top.innerHTML = "";

      for (const system_id in data) {
        const system = data[system_id];
        var div = document.createElement('div');
        var name = document.createElement('h3');
        name.innerHTML = system['name'];
        var graph = document.createElement('div');
        div.appendChild(name);
        div.appendChild(graph);
        top.appendChild(div);
        console.log(window.innerHeight / data.length)
        const Graph = new ForceGraph(graph)
          .graphData(system['graphs'][0])
          .nodeId('id')
          .nodeVal('val')
          .nodeLabel('id')
          .nodeAutoColorBy('server')
          .linkSource('source')
          .linkTarget('target')
          .linkDirectionalParticles(2)
          .nodeRelSize(1)
          .dagMode('lr')
          .dagLevelDistance(30)
          .height(window.innerHeight / data.length)
      }
    });
}

function fetchServers() {
    fetch('/api/servers')
     .then(res => res.json()).then(data => {
        console.log(data);
        const stats = ['cpu', 'memory', 'network', 'disk'];
        for (const system in data) {
              var top = document.createElement('p');
          top.innerHTML = "<h3>" + data[system]['name'] + ":</h3>";
          for (const serv in data[system]['servers']) {
              var a = document.createElement('p');
              top.appendChild(a);
              for (const ind in stats) {
                  const usage = data[system]['servers'][serv]['usage'][stats[ind]];
                  const limits = data[system]['servers'][serv]['limits'][stats[ind]];
                  a.innerHTML += stats[ind] + ": " + usage + '/' + limits 
                    + `<progress value=${usage/limits}></progress>` + "<br>";
              }
              document.getElementById('root').appendChild(top);
          }
        }
    });

}
