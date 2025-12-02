const API_STATS = '/api/stats';
const API_HISTORY = '/api/history';
const INTERVAL = 1000;
let currentDetailId = null;
let chartInstances = {};

function formatBytes(bytes) {
    if (bytes === 0) return '0 B';
    const k = 1024;
    const sizes = ['B', 'KB', 'MB', 'GB', 'TB'];
    const i = Math.floor(Math.log(bytes) / Math.log(k));
    return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i];
}

function formatSpeed(bytes) {
    return formatBytes(bytes) + '/s';
}

document.addEventListener('DOMContentLoaded', () => {
    Chart.defaults.color = '#a0aec0';
    Chart.defaults.borderColor = '#4a5568';
    
    fetchData();
    setInterval(fetchData, INTERVAL);
});

async function fetchData() {
    try {
        const res = await fetch(API_STATS);
        if (!res.ok) throw new Error("Network response was not ok");
        const data = await res.json();
        
        if (!currentDetailId) {
            renderGrid(data);
        } else {
            // TODO:
            // implement detail page data update here
            // SOON?
        }
    } catch (error) {
        console.error("Fetch stats failed:", error);
    }
}

function renderGrid(data) {
    const grid = document.getElementById('server-grid');
    
    if (data.length === 0) {
        grid.innerHTML = '<div style="grid-column: 1/-1; text-align: center; padding: 40px; color: #718096;">No servers online</div>';
        return;
    }

    grid.innerHTML = data.map(server => createCardHTML(server)).join('');
}

function createCardHTML(s) {
    const cpuColor = s.cpu > 90 ? 'crit' : (s.cpu > 70 ? 'high' : '');
    const ramPercent = (s.mem_used / s.mem_total) * 100;
    const ramColor = ramPercent > 90 ? 'crit' : (ramPercent > 70 ? 'high' : '');
    const diskPercent = (s.disk_used / s.disk_total) * 100;

    // TODO: display national flag
    const flag = "🇨🇳"; 

    
    return `
    <div class="card" onclick="openDetail('${s.id}')">
        <div class="card-header">
            <span class="flag-icon" style="display:flex;align-items:center;justify-content:center;background:none;font-size:1.2rem;">${flag}</span>
            <div class="server-name">${s.id}</div>
            <div class="status-dot"></div>
        </div>
        
        <div class="stat-row">
            <div class="stat-label">
                <span>CPU</span>
                <span>${s.cpu.toFixed(1)}%</span>
            </div>
            <div class="progress-track">
                <div class="progress-fill ${cpuColor}" style="width: ${s.cpu}%"></div>
            </div>
        </div>

        <div class="stat-row">
            <div class="stat-label">
                <span>RAM</span>
                <span>${Math.round(ramPercent)}%</span>
            </div>
            <div class="progress-track">
                <div class="progress-fill ${ramColor}" style="width: ${ramPercent}%"></div>
            </div>
        </div>

        <div class="stat-row">
            <div class="stat-label">
                <span>Disk</span>
                <span>${Math.round(diskPercent)}%</span>
            </div>
            <div class="progress-track">
                <div class="progress-fill" style="width: ${diskPercent}%"></div>
            </div>
        </div>

        <div class="net-stats">
            <div class="net-item" title="Download">
                <span class="net-icon">↓</span>
                <span>${formatSpeed(s.rx_speed)}</span>
            </div>
            <div class="net-item" title="Upload">
                <span class="net-icon">↑</span>
                <span>${formatSpeed(s.tx_speed)}</span>
            </div>
        </div>
    </div>
    `;
}


window.openDetail = function(id) {
    currentDetailId = id;
    document.getElementById('dashboard-view').style.display = 'none';
    document.getElementById('detail-view').style.display = 'block';
    document.getElementById('detail-title').innerText = id;
    
    loadHistory(id);

    if (window.detailInterval) clearInterval(window.detailInterval);
    window.detailInterval = setInterval(() => loadHistory(id), 2000);
};

window.backToDashboard = function() {
    currentDetailId = null;
    if (window.detailInterval) clearInterval(window.detailInterval);
    document.getElementById('detail-view').style.display = 'none';
    document.getElementById('dashboard-view').style.display = 'block';
};

async function loadHistory(id) {
    try {
        const res = await fetch(`${API_HISTORY}?id=${id}`);
        if (!res.ok) return;
        const history = await res.json();
        updateCharts(history);
    } catch (e) {
        console.error(e);
    }
}

function updateCharts(data) {
    const labels = data.map(() => '');
    const cpuData = data.map(d => d.cpu);
    const memData = data.map(d => (d.mem_used / d.mem_total) * 100);
    const netRx = data.map(d => d.rx_speed / 1024); // KB/s
    const netTx = data.map(d => d.tx_speed / 1024);

    const chartConfig = {
        responsive: true,
        maintainAspectRatio: false,
        animation: false,
        elements: { point: { radius: 0, hitRadius: 10 } },
        scales: {
            x: { grid: { display: false } },
            y: { grid: { color: '#2d3748' }, beginAtZero: true } 
        }
    };

    // 1. CPU & RAM Chart
    const ctx1 = document.getElementById('chart-cpu-mem').getContext('2d');
    if (!chartInstances.cpu) {
        chartInstances.cpu = new Chart(ctx1, {
            type: 'line',
            data: {
                labels: labels,
                datasets: [
                    { label: 'CPU %', data: cpuData, borderColor: '#48bb78', backgroundColor: 'rgba(72, 187, 120, 0.1)', fill: true, borderWidth: 2 },
                    { label: 'RAM %', data: memData, borderColor: '#667eea', backgroundColor: 'rgba(102, 126, 234, 0.1)', fill: true, borderWidth: 2 }
                ]
            },
            options: { ...chartConfig, plugins: { title: { display: true, text: 'Resource Usage' } } }
        });
    } else {
        chartInstances.cpu.data.labels = labels;
        chartInstances.cpu.data.datasets[0].data = cpuData;
        chartInstances.cpu.data.datasets[1].data = memData;
        chartInstances.cpu.update('none');
    }

    // 2. Network Chart
    const ctx2 = document.getElementById('chart-net').getContext('2d');
    if (!chartInstances.net) {
        chartInstances.net = new Chart(ctx2, {
            type: 'line',
            data: {
                labels: labels,
                datasets: [
                    { label: 'In (KB/s)', data: netRx, borderColor: '#ecc94b', backgroundColor: 'rgba(236, 201, 75, 0.1)', fill: true, borderWidth: 2 },
                    { label: 'Out (KB/s)', data: netTx, borderColor: '#f56565', backgroundColor: 'rgba(245, 101, 101, 0.1)', fill: true, borderWidth: 2 }
                ]
            },
            options: { ...chartConfig, plugins: { title: { display: true, text: 'Network Traffic' } } }
        });
    } else {
        chartInstances.net.data.labels = labels;
        chartInstances.net.data.datasets[0].data = netRx;
        chartInstances.net.data.datasets[1].data = netTx;
        chartInstances.net.update('none');
    }
}