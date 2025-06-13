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
      var table = '<thead><tr><th scope="col">#</th><th scope="col">Name</th><th scope="col">cpu</th><th scope="col">memory</th><th scope="col">network</th><!--<th scope="col">disk</th>--></tr></thead><tbody>';
            document.getElementById('server-root').innerHTML = '';
            console.log(data);
            const stats = ['cpu', 'memory', 'network'/*, 'disk'*/];
            for (const system in data) {
                var top = document.createElement('p');
                top.innerHTML = "<h3>" + data[system]['name'] + ":</h3>";
                var stats_total = [{min: 1e8, max: 0, sum:0}, {min: 1e8, max: 0, sum:0}, {min: 1e8, max: 0, sum:0}, {min: 1e8, max: 0, sum:0}];
                for (const serv in data[system]['servers']) {
                    var a = document.createElement('p');
                    top.appendChild(a);
                    for (const ind in stats) {
                        const usage = data[system]['servers'][serv]['usage'][stats[ind]];
                        const limits = data[system]['servers'][serv]['limits'][stats[ind]];
                        a.innerHTML += stats[ind] + ": " + usage + '/' + limits +
                            `<progress value=${usage/limits}></progress>` + "<br>";
                        stats_total[ind].sum += usage;
                        stats_total[ind].min = Math.min(stats_total[ind].min, usage);
                        stats_total[ind].max = Math.max(stats_total[ind].max, usage);
                    }
                    document.getElementById('server-root').appendChild(top);
                }
                table += `<tr><th scope="row">${system}</th><td>${data[system]['name']}</td>`
                console.log(stats_total);
                for (const ind in stats) {
                    table += `<td>${stats_total[ind].sum.toFixed(2)}/${((stats_total[ind].max - stats_total[ind].min) * 100 / stats_total[ind].sum).toFixed(2)}%</td>`
        }
            }
            table +='</tbody>';
            document.getElementById('server-table').innerHTML = table;
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
            
            document.getElementById('max_count_local').value = data.max_count_local;

            document.getElementById('max_distributed_traffic').value = data.max_distributed_traffic;

        });
}

function submitFormData() {
    const queryParams = [
        `graph_size=${document.getElementById('graph_size_min').value},${document.getElementById('graph_size_max').value}`,
        `source_volume=${document.getElementById('source_volume_min').value},${document.getElementById('source_volume_max').value}`,
        `filter_volume=${document.getElementById('filter_volume_min').value},${document.getElementById('filter_volume_max').value}`,
        `servers_count=${ document.getElementById('servers_count').value}`,
        `servers_stat=${document.getElementById('server_cpu').value},${document.getElementById('server_memory').value},${document.getElementById('server_network').value},${document.getElementById('server_disk').value}`,
        `max_count_local=${document.getElementById('max_count_local').value}`
        `max_distributed_traffic=${document.getElementById('max_distributed_traffic').value}`
    ].join('&');
    fetch(`/api/params?${queryParams}`, {
        method: "POST"
    }).then(fetchFormData);
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
