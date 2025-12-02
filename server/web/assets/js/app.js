const API_STATS = '/api/stats';
const API_HISTORY = '/api/history';
const INTERVAL = 1000;
let currentDetailId = null;
let chartInstances = {};

const CHART_COLORS = [
    '#3b82f6', // blue
    '#ef4444', // red
    '#10b981', // green
    '#f59e0b', // yellow
    '#8b5cf6', // purple
    '#ec4899', // pink
    '#06b6d4'  // cyan
];

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
    Chart.defaults.font.family = "'Segoe UI', 'Roboto', sans-serif";
    
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
        }
    } catch (error) {
        console.error("Fetch stats failed:", error);
    }
}

function renderGrid(data) {
    const grid = document.getElementById('server-grid');
    if (!grid) return;
    
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
    const dashboard = document.getElementById('dashboard-view');
    const detail = document.getElementById('detail-view');
    
    if(dashboard) dashboard.style.display = 'none';
    if(detail) {
        detail.style.display = 'block';
        document.getElementById('detail-title').innerText = id;
    }
    
    if (chartInstances.speedtest) {
        chartInstances.speedtest.destroy();
        chartInstances.speedtest = null;

        const oldStats = document.getElementById('speedtest-stats-header');
        if(oldStats) oldStats.remove();
    }

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
    // assume data is ordered by time ascending
    // use index as X tag
    // if timestamp is available, use this ->
    // new Date(d.timestamp).toLocaleTimeString()
    const labels = data.map((_, i) => i); 

    const cpuData = data.map(d => d.cpu);
    const memData = data.map(d => (d.mem_used / d.mem_total) * 100);
    const netRx = data.map(d => d.rx_speed / 1024); // KB/s
    const netTx = data.map(d => d.tx_speed / 1024);

    const commonOptions = {
        responsive: true,
        maintainAspectRatio: false,
        animation: false,
        interaction: {
            mode: 'index',
            intersect: false,
        },
        elements: { 
            point: { radius: 0, hitRadius: 10, hoverRadius: 4 },
            line: { borderWidth: 1.5, tension: 0.3 }
        },
        plugins: {
            legend: { labels: { color: '#a0aec0', boxWidth: 10, font: { size: 11 } } },
            tooltip: { 
                backgroundColor: 'rgba(26, 32, 44, 0.9)',
                titleColor: '#e2e8f0',
                bodyColor: '#cbd5e0',
                borderColor: '#4a5568',
                borderWidth: 1
            }
        },
        scales: {
            x: { 
                grid: { display: false, drawBorder: false }, 
                ticks: { 
                    color: '#718096', 
                    maxTicksLimit: 8, // max x
                    maxRotation: 0
                } 
            },
            y: { 
                grid: { color: '#2d3748', borderDash: [4, 4] }, 
                ticks: { color: '#718096', maxTicksLimit: 5 }, // max y
                beginAtZero: true 
            }
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
                    { label: 'CPU %', data: cpuData, borderColor: '#48bb78', backgroundColor: 'rgba(72, 187, 120, 0.1)', fill: true },
                    { label: 'RAM %', data: memData, borderColor: '#667eea', backgroundColor: 'rgba(102, 126, 234, 0.1)', fill: true }
                ]
            },
            options: { 
                ...commonOptions, 
                plugins: { ...commonOptions.plugins, title: { display: true, text: 'Resource Usage', color: '#e2e8f0' } } 
            }
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
                    { label: 'In (KB/s)', data: netRx, borderColor: '#ecc94b', backgroundColor: 'rgba(236, 201, 75, 0.1)', fill: true },
                    { label: 'Out (KB/s)', data: netTx, borderColor: '#f56565', backgroundColor: 'rgba(245, 101, 101, 0.1)', fill: true }
                ]
            },
            options: { 
                ...commonOptions, 
                plugins: { ...commonOptions.plugins, title: { display: true, text: 'Network Traffic', color: '#e2e8f0' } } 
            }
        });
    } else {
        chartInstances.net.data.labels = labels;
        chartInstances.net.data.datasets[0].data = netRx;
        chartInstances.net.data.datasets[1].data = netTx;
        chartInstances.net.update('none');
    }

    const speedtestCanvas = document.getElementById('chart-speedtest');
    if (speedtestCanvas) {
        const targetStats = {}; // { "TargetName": { sum: 0, count: 0, data: [], color: '' } }
        
        data.forEach(point => {
            if (point.speedtest && Array.isArray(point.speedtest)) {
                point.speedtest.forEach(res => {
                    if (!targetStats[res.id]) {
                        targetStats[res.id] = { sum: 0, count: 0, data: new Array(data.length).fill(null) };
                    }
                });
            }
        });

        const targetNames = Object.keys(targetStats);
        
        data.forEach((point, idx) => {
            if (point.speedtest && Array.isArray(point.speedtest)) {
                point.speedtest.forEach(res => {
                    if (targetStats[res.id]) {
                        const val = res.latency > 0 ? res.latency : null;
                        targetStats[res.id].data[idx] = val;

                        // exclude nulls from average calculation
                        if (val !== null) {
                            targetStats[res.id].sum += val;
                            targetStats[res.id].count++;
                        }
                    }
                });
            }
        });

        renderSpeedtestHeader(speedtestCanvas, targetStats, targetNames);

        const datasets = targetNames.map((name, i) => {
            const color = CHART_COLORS[i % CHART_COLORS.length];
            targetStats[name].color = color; 
            return {
                label: name,
                data: targetStats[name].data,
                borderColor: color,
                backgroundColor: color,
                borderWidth: 1.5,
                pointRadius: 0,
                hoverRadius: 4,
                tension: 0.1,
                spanGaps: true
            };
        });

        const ctx3 = speedtestCanvas.getContext('2d');
        if (!chartInstances.speedtest) {
            chartInstances.speedtest = new Chart(ctx3, {
                type: 'line',
                data: { labels: labels, datasets: datasets },
                options: {
                    ...commonOptions,
                    plugins: {
                        ...commonOptions.plugins,
                        title: { display: false },
                        tooltip: {
                            ...commonOptions.plugins.tooltip,
                            callbacks: {
                                label: function(context) {
                                    return ` ${context.dataset.label}: ${context.parsed.y.toFixed(2)} ms`;
                                }
                            }
                        }
                    },
                    scales: {
                        x: { 
                            ...commonOptions.scales.x,
                            ticks: { ...commonOptions.scales.x.ticks, maxTicksLimit: 10 } 
                        },
                        y: { 
                            ...commonOptions.scales.y,
                            title: { display: true, text: 'Latency (ms)', color: '#718096', font: {size: 10} }
                        }
                    }
                }
            });
        } else {
            chartInstances.speedtest.data.labels = labels;
            chartInstances.speedtest.data.datasets = datasets;
            chartInstances.speedtest.update('none');

            updateHeaderColors(targetStats);
        }
    }
}

function renderSpeedtestHeader(canvasElement, stats, names) {
    let header = document.getElementById('speedtest-stats-header');
    
    // insert to Canvas
    if (!header) {
        header = document.createElement('div');
        header.id = 'speedtest-stats-header';
        header.style.cssText = `
            display: flex;
            flex-wrap: wrap;
            gap: 20px;
            margin-bottom: 15px;
            padding-bottom: 15px;
            border-bottom: 1px solid #2d3748;
        `;
        canvasElement.parentNode.insertBefore(header, canvasElement);
    }

    header.innerHTML = names.map((name, i) => {
        const obj = stats[name];
        const avg = obj.count > 0 ? (obj.sum / obj.count).toFixed(2) : "0.00";
        // assign color dynamically
        const color = CHART_COLORS[i % CHART_COLORS.length];
        
        return `
            <div style="flex: 1; min-width: 120px;">
                <div style="font-size: 0.8rem; color: #a0aec0; margin-bottom: 4px; display: flex; align-items: center;">
                    <span style="width: 8px; height: 8px; border-radius: 50%; background-color: ${color}; margin-right: 6px; display: inline-block;"></span>
                    ${name}
                </div>
                <div style="font-size: 1.5rem; font-weight: 600; color: #e2e8f0; line-height: 1.2;">
                    ${avg}<span style="font-size: 0.9rem; font-weight: normal; color: #718096; margin-left: 2px;">ms</span>
                </div>
                <div style="font-size: 0.75rem; color: #718096;">
                    0% loss
                </div>
            </div>
        `;
    }).join('');
}

function updateHeaderColors(stats) {
    // if you want to update colors dynamically, implement here
}