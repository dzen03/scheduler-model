function fetchGraph() {
  fetch(`/api/graphs?id=${document.getElementById('index').value}`)
    .then(res => res.json()).then(data => {
      const Graph = new ForceGraph(document.getElementById('graph'))
        .graphData(data[0])
        .nodeId('id')
        .nodeVal('val')
        .nodeLabel('id')
        .nodeAutoColorBy('server')
        .linkSource('source')
        .linkTarget('target')
        .linkDirectionalParticles(2)
        .nodeRelSize(4)
    });
}

function fetchServers() {
    fetch('/api/servers')
     .then(res => res.json()).then(data => {
        const stats = ['cpu', 'memory', 'network', 'disk'];
        for (const serv in data) {
            var a = document.createElement('p');
            for (const ind in stats) {
                const usage = data[serv]['usage'][stats[ind]];
                const limits = data[serv]['limits'][stats[ind]];
                a.innerHTML += stats[ind] + ": " + usage + '/' + limits 
                  + `<progress value=${usage/limits}></progress>` + "<br>";
            }
            document.getElementById('root').appendChild(a);
        }
    });

}
