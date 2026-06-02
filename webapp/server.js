const express = require('express');
const fs = require('fs');
const path = require('path');
const morgan = require('morgan');
const cors = require('cors');

const app = express();
const port = process.env.PORT || 3000;
const dataDir = path.join(__dirname, 'data');
const txtDir = path.join(__dirname, '..', 'bloodonationsystem');

const files = {
  donorsTxt: path.join(txtDir, 'donors.txt'),
  requestsTxt: path.join(txtDir, 'requests.txt'),
  historyTxt: path.join(txtDir, 'history.txt'),
  stockTxt: path.join(txtDir, 'blood_stock.txt'),
  banks: path.join(dataDir, 'banks.json')
};

const validBloodGroups = ['A+', 'A-', 'B+', 'B-', 'O+', 'O-', 'AB+', 'AB-'];

function ensureDataDirectory() {
  if (!fs.existsSync(dataDir)) {
    fs.mkdirSync(dataDir, { recursive: true });
  }
  if (!fs.existsSync(txtDir)) {
    fs.mkdirSync(txtDir, { recursive: true });
  }
}

function readJson(filePath, defaultValue) {
  try {
    if (!fs.existsSync(filePath)) {
      return defaultValue;
    }
    const content = fs.readFileSync(filePath, 'utf8');
    return content.trim() ? JSON.parse(content) : defaultValue;
  } catch (err) {
    console.error('Failed to read', filePath, err);
    return defaultValue;
  }
}

function writeJson(filePath, data) {
  fs.writeFileSync(filePath, JSON.stringify(data, null, 2), 'utf8');
}

function normalizeBloodGroup(bg) {
  if (!bg || typeof bg !== 'string') return '';
  return bg.toUpperCase().replace(/\s+/g, '');
}

function isValidBloodGroup(bg) {
  return validBloodGroups.includes(bg);
}

function getCompatibleDonorGroups(bg) {
  const group = normalizeBloodGroup(bg);
  switch (group) {
    case 'A+': return ['A+', 'A-', 'O+', 'O-'];
    case 'A-': return ['A-', 'O-'];
    case 'B+': return ['B+', 'B-', 'O+', 'O-'];
    case 'B-': return ['B-', 'O-'];
    case 'AB+': return ['A+', 'A-', 'B+', 'B-', 'AB+', 'AB-', 'O+', 'O-'];
    case 'AB-': return ['A-', 'B-', 'AB-', 'O-'];
    case 'O+': return ['O+', 'O-'];
    case 'O-': return ['O-'];
    default: return [];
  }
}

function readTxt(filePath, parser, defaultValue) {
  try {
    if (!fs.existsSync(filePath)) return defaultValue;
    const content = fs.readFileSync(filePath, 'utf8');
    const lines = content.split(/\r?\n/).map(line => line.trim()).filter(line => line.length > 0);
    const items = lines.map(parser).filter(Boolean);
    return items.length ? items : defaultValue;
  } catch (err) {
    console.error('Failed to read txt', filePath, err);
    return defaultValue;
  }
}

function writeTxt(filePath, lines) {
  fs.writeFileSync(filePath, lines.join('\n') + (lines.length ? '\n' : ''), 'utf8');
}

function parseDonorLine(line) {
  const parts = line.split('|').map(part => part.trim());
  if (parts.length < 7) return null;
  return {
    name: parts[0] || 'Unknown',
    age: Number(parts[1]) || 0,
    bloodGroup: normalizeBloodGroup(parts[2]),
    city: parts[3] || 'Unknown',
    contact: parts[4] || 'Unknown',
    lastDonationDate: parts[5] || '',
    disease: parts[6] || 'None',
    cnic: parts[7] || ''
  };
}

function parseRequestLine(line) {
  const parts = line.split('|').map(part => part.trim());
  if (parts.length < 4) return null;
  return {
    patientName: parts[0] || 'Unknown',
    bloodGroup: normalizeBloodGroup(parts[1]),
    unitsNeeded: Number(parts[2]) || 0,
    hospital: parts[3] || 'Unknown',
    patientHealth: parts[4] || 'None',
    createdAt: parts[5] || new Date().toISOString()
  };
}

function parseHistoryLine(line) {
  const parts = line.split('|').map(part => part.trim());
  if (parts.length < 3) return null;
  return {
    donorName: parts[0] || 'Unknown',
    cnic: parts[1] || '',
    bloodGroup: normalizeBloodGroup(parts[2]),
    date: parts[3] || ''
  };
}

function formatHistoryLine(record) {
  return [record.donorName, record.cnic || '', record.bloodGroup, record.date].join('|');
}

function formatDonorLine(donor) {
  return [donor.name, donor.age, donor.bloodGroup, donor.city, donor.contact, donor.lastDonationDate, donor.disease || 'None', donor.cnic || ''].join('|');
}

function formatRequestLine(request) {
  return [request.patientName, request.bloodGroup, request.unitsNeeded, request.hospital, request.patientHealth || 'None', request.createdAt || new Date().toISOString()].join('|');
}

function formatStockLines(stock) {
  return validBloodGroups.map(bg => `${bg}|${stock[bg] || 0}`);
}

function readDonors() {
  return readTxt(files.donorsTxt, parseDonorLine, []);
}

function writeDonors(donors) {
  writeTxt(files.donorsTxt, donors.map(formatDonorLine));
}

function readRequests() {
  return readTxt(files.requestsTxt, parseRequestLine, []);
}

function writeRequests(requests) {
  writeTxt(files.requestsTxt, requests.map(formatRequestLine));
}

function readHistory() {
  return readTxt(files.historyTxt, parseHistoryLine, []);
}

function writeHistory(history) {
  writeTxt(files.historyTxt, history.map(formatHistoryLine));
}

function readStock() {
  const stock = readTxt(files.stockTxt, line => {
    const parts = line.split('|').map(part => part.trim());
    if (parts.length < 2) return null;
    const bg = normalizeBloodGroup(parts[0]);
    if (!isValidBloodGroup(bg)) return null;
    return { group: bg, units: Number(parts[1]) || 0 };
  }, []);
  if (!stock || stock.length === 0) {
    return { 'A+': 50, 'A-': 30, 'B+': 40, 'B-': 25, 'O+': 60, 'O-': 35, 'AB+': 20, 'AB-': 15 };
  }
  return stock.reduce((acc, item) => {
    if (item && item.group) acc[item.group] = item.units;
    return acc;
  }, {});
}

function writeStock(stock) {
  writeTxt(files.stockTxt, formatStockLines(stock));
}

function initData() {
  ensureDataDirectory();

  const defaultStock = { 'A+': 50, 'A-': 30, 'B+': 40, 'B-': 25, 'O+': 60, 'O-': 35, 'AB+': 20, 'AB-': 15 };

  const defaultBanks = {
    banks: [
      { name: 'Rawalpindi General Hospital Blood Bank', lat: 33.5974, lng: 73.0548 },
      { name: 'Holy Family Hospital Rawalpindi Blood Bank', lat: 33.5786, lng: 73.0500 },
      { name: 'Al-Shifa Trust Hospital Rawalpindi Blood Bank', lat: 33.6140, lng: 73.0616 },
      { name: 'District Headquarters Hospital Blood Bank', lat: 33.6180, lng: 73.0440 },
      { name: 'Benazir Bhutto Hospital Blood Bank', lat: 33.6320, lng: 73.0300 }
    ]
  };

  if (!fs.existsSync(files.donorsTxt)) writeDonors([]);
  if (!fs.existsSync(files.requestsTxt)) writeRequests([]);
  if (!fs.existsSync(files.historyTxt)) writeHistory([]);
  if (!fs.existsSync(files.stockTxt)) writeStock(defaultStock);
  if (!fs.existsSync(files.banks)) writeJson(files.banks, defaultBanks);
}

app.use(morgan('dev'));
app.use(cors());
app.use(express.json());
app.use(express.static(path.join(__dirname, 'public')));
app.use((req, res, next) => {
  res.setHeader('Cache-Control', 'no-store');
  next();
});

initData();

app.get('/health', (req, res) => {
  res.json({ status: 'ok', uptime: process.uptime(), timestamp: Date.now() });
});

app.get('/api/donors', (req, res) => {
  const donors = readDonors();
  res.json(donors);
});

app.post('/api/donors', (req, res) => {
  const { name, age, bloodGroup, city, contact, lastDonationDate, disease, cnic } = req.body;
  if (!name || !city || !contact || !lastDonationDate || typeof age !== 'number' || !cnic) {
    return res.status(400).json({ error: 'Please provide all donor fields.' });
  }
  const bg = normalizeBloodGroup(bloodGroup);
  if (!isValidBloodGroup(bg)) {
    return res.status(400).json({ error: 'Invalid blood group.' });
  }
  const donors = readDonors();
  donors.push({ name, age, bloodGroup: bg, city, contact, lastDonationDate, disease: disease || 'None', cnic });
  writeDonors(donors);
  res.json({ success: true, donors });
});

app.delete('/api/donors/:name', (req, res) => {
  const name = req.params.name;
  const donors = readDonors();
  const filtered = donors.filter(d => d.name.toLowerCase() !== name.toLowerCase());
  writeDonors(filtered);
  res.json({ success: true, donors: filtered });
});

app.get('/api/donors/search', (req, res) => {
  const term = (req.query.q || '').toLowerCase();
  const donors = readDonors();
  if (!term) return res.json(donors);
  const results = donors.filter(d => (d.name || '').toLowerCase().includes(term) || (d.cnic || '').toLowerCase().includes(term));
  res.json(results);
});

app.get('/api/donors/blood-group/:bg', (req, res) => {
  const bg = normalizeBloodGroup(req.params.bg);
  const donors = readDonors();
  const results = donors.filter(d => d.bloodGroup === bg);
  res.json(results);
});

app.get('/api/requests', (req, res) => {
  const requests = readRequests();
  res.json(requests);
});

app.post('/api/requests', (req, res) => {
  const { patientName, bloodGroup, unitsNeeded, hospital, patientHealth } = req.body;
  if (!patientName || !hospital || typeof unitsNeeded !== 'number') {
    return res.status(400).json({ error: 'Please provide valid request data.' });
  }
  const bg = normalizeBloodGroup(bloodGroup);
  if (!isValidBloodGroup(bg)) {
    return res.status(400).json({ error: 'Invalid blood group.' });
  }
  const requests = readRequests();
  requests.push({ patientName, bloodGroup: bg, unitsNeeded, hospital, patientHealth: patientHealth || 'None', createdAt: new Date().toISOString() });
  writeRequests(requests);
  res.json({ success: true, requests });
});

app.post('/api/requests/process', (req, res) => {
  const stock = readStock();
  const requests = readRequests();
  if (requests.length === 0) {
    return res.status(400).json({ error: 'No pending requests.' });
  }
  const next = requests[0];
  const bg = next.bloodGroup;
  if (!stock[bg] || stock[bg] < next.unitsNeeded) {
    return res.status(400).json({ error: 'Insufficient stock to process the next request.' });
  }
  stock[bg] -= next.unitsNeeded;
  requests.shift();
  writeStock(stock);
  writeRequests(requests);
  res.json({ success: true, processed: next, stock, requests });
});

app.get('/api/history', (req, res) => {
  const history = readHistory();
  res.json(history);
});

app.post('/api/history', (req, res) => {
  const { donorName, cnic, bloodGroup, date } = req.body;
  if (!donorName || !cnic || !date) {
    return res.status(400).json({ error: 'Please provide donor name, CNIC, and date.' });
  }
  const bg = normalizeBloodGroup(bloodGroup);
  if (!isValidBloodGroup(bg)) {
    return res.status(400).json({ error: 'Invalid blood group.' });
  }
  const history = readHistory();
  history.unshift({ donorName, cnic, bloodGroup: bg, date });
  writeHistory(history);
  res.json({ success: true, history });
});

app.get('/api/stock', (req, res) => {
  const stock = readStock();
  res.json(stock);
});

app.post('/api/stock/update', (req, res) => {
  const { bloodGroup, units } = req.body;
  const bg = normalizeBloodGroup(bloodGroup);
  if (!isValidBloodGroup(bg) || typeof units !== 'number') {
    return res.status(400).json({ error: 'Invalid stock update request.' });
  }
  const stock = readStock();
  stock[bg] = (stock[bg] || 0) + units;
  writeStock(stock);
  res.json({ success: true, stock });
});

app.get('/api/stock/compatibility/:bg', (req, res) => {
  const groups = getCompatibleDonorGroups(req.params.bg);
  res.json(groups);
});

function haversineDistance(lat1, lon1, lat2, lon2) {
  const toRad = deg => deg * Math.PI / 180;
  const R = 6371;
  const dLat = toRad(lat2 - lat1);
  const dLon = toRad(lon2 - lon1);
  const a = Math.sin(dLat / 2) ** 2 + Math.cos(toRad(lat1)) * Math.cos(toRad(lat2)) * Math.sin(dLon / 2) ** 2;
  const c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));
  return R * c;
}

function buildBankRoutes(banks) {
  const routes = [];
  for (let i = 0; i < banks.length; i++) {
    for (let j = i + 1; j < banks.length; j++) {
      const distance = haversineDistance(banks[i].lat, banks[i].lng, banks[j].lat, banks[j].lng);
      routes.push({ from: i, to: j, distance: Number(distance.toFixed(1)) });
    }
  }
  return routes.sort((a, b) => a.distance - b.distance);
}

app.get('/api/banks', (req, res) => {
  const data = readJson(files.banks, { banks: [] });
  const banks = data.banks || [];
  const routes = buildBankRoutes(banks);
  res.json({ banks, routes });
});

app.get('/api/banks/closest', (req, res) => {
  const bankNameRaw = (req.query.bank || '').trim();
  const bankName = bankNameRaw.toLowerCase();
  const data = readJson(files.banks, { banks: [] });
  const banks = data.banks || [];
  let index = banks.findIndex(b => b.name.toLowerCase() === bankName);
  if (index === -1) {
    index = banks.findIndex(b => b.name.toLowerCase().includes(bankName));
  }
  if (index === -1) {
    return res.status(400).json({ error: 'Bank not found.' });
  }
  let closest = null;
  for (let i = 0; i < banks.length; i++) {
    if (i === index) continue;
    const distance = haversineDistance(banks[index].lat, banks[index].lng, banks[i].lat, banks[i].lng);
    if (!closest || distance < closest.distance) {
      closest = { bank: banks[i].name, distance: Number(distance.toFixed(1)) };
    }
  }
  if (!closest) {
    return res.status(400).json({ error: 'No connected banks found.' });
  }
  res.json({ bank: banks[index].name, closest });
});

app.get('/api/summary', (req, res) => {
  const donors = readDonors();
  const requests = readRequests();
  const history = readHistory();
  const stock = readStock();
  res.json({ donorsCount: donors.length, pendingRequests: requests.length, historyCount: history.length, stock });
});

app.get('*', (req, res) => {
  res.sendFile(path.join(__dirname, 'public', 'index.html'));
});

const server = app.listen(port, () => {
  console.log(`Blood Donation web app running at http://localhost:${port}`);
});

function shutdown(signal) {
  console.log(`Received ${signal}, shutting down gracefully...`);
  server.close(() => {
    console.log('Server closed.');
    process.exit(0);
  });
  setTimeout(() => {
    console.error('Forcing shutdown.');
    process.exit(1);
  }, 5000).unref();
}

process.on('SIGINT', () => shutdown('SIGINT'));
process.on('SIGTERM', () => shutdown('SIGTERM'));

app.use((err, req, res, next) => {
  console.error('Unhandled error:', err);
  res.status(500).json({ error: 'Internal server error' });
});
