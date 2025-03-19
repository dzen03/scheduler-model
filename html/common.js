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
      document.getElementById('server-root').innerHTML = '';
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
              document.getElementById('server-root').appendChild(top);
          }
        }
    });

}

function addNodes() {
  fetch(`/api/addnodes?count=${document.getElementById('count').value}`);
}
function reset() {
  fetch(`/api/reset`);
}


function fetchFormData() {

       fetch('/api/params')
  .then(res => res.json()).then(data => {
            // Graph Size
        document.getElementById('graph_size_min').value = data.graph_size.min;
        document.getElementById('graph_size_max').value = data.graph_size.max;

        // Source Volume
        document.getElementById('source_volume_min').value = data.source_volume.min;
        document.getElementById('source_volume_max').value = data.source_volume.max;

        // Filter Volume
        document.getElementById('filter_volume_min').value = data.filter_volume.min;
        document.getElementById('filter_volume_max').value = data.filter_volume.max;

        // Servers Count
        document.getElementById('servers_count').value = data.servers_count;

        // Servers Stat
        document.getElementById('server_cpu').value = data.servers_stat.cpu;
        document.getElementById('server_memory').value = data.servers_stat.memory;
        document.getElementById('server_network').value = data.servers_stat.network;
        document.getElementById('server_disk').value = data.servers_stat.disk;

    });
}

function submitFormData() {
  const queryParams = [
        `graph_size=${document.getElementById('graph_size_min').value},${document.getElementById('graph_size_max').value}`,
        `source_volume=${document.getElementById('source_volume_min').value},${document.getElementById('source_volume_max').value}`,
        `filter_volume=${document.getElementById('filter_volume_min').value},${document.getElementById('filter_volume_max').value}`,
        `servers_count=${ document.getElementById('servers_count').value}`,
        `servers_stat=${document.getElementById('server_cpu').value},${document.getElementById('server_memory').value},${document.getElementById('server_network').value},${document.getElementById('server_disk').value}`
      ].join('&');
  fetch(`/api/params?${queryParams}`, {method: "POST"}).then(fetchFormData);
 return false; 
}

function fetchContent() {
  fetchServers();
  //fetchFormData();
}
function OnLoad() {
  fetchFormData();
  fetchServers();
  window.setInterval(fetchContent, 500);
}
