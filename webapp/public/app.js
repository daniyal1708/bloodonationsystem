const panels = document.querySelectorAll('.panel');
const navButtons = document.querySelectorAll('.button-row button');
const summaryCards = document.getElementById('summaryCards');
const donorForm = document.getElementById('donorForm');
const donorTable = document.getElementById('donorTable');
const loadDonorsButton = document.getElementById('loadDonors');
const donorSearch = document.getElementById('donorSearch');
const requestForm = document.getElementById('requestForm');
const requestTable = document.getElementById('requestTable');
const processRequestButton = document.getElementById('processRequest');
const historyForm = document.getElementById('historyForm');
const historyTable = document.getElementById('historyTable');
const stockTable = document.getElementById('stockTable');
const updateStockButton = document.getElementById('updateStock');
const stockGroupInput = document.getElementById('stockGroup');
const compatibleResult = document.getElementById('compatibleResult');
const compatibilityCheck = document.getElementById('compatibilityCheck');
const messageBox = document.getElementById('messageBox');
const findClosest = document.getElementById('findClosest');
const currentBankInput = document.getElementById('currentBank');
const closestBankResult = document.getElementById('closestBankResult');
const bankInfoBox = document.getElementById('bankInfoBox');
const bankTable = document.getElementById('bankTable');
const routeTable = document.getElementById('routeTable');

async function fetchJson(url, options = {}) {
  options = options || {};
  options.headers = options.headers || {};
  const response = await fetch(url, options);
  if (!response.ok) {
    const error = await response.json().catch(() => ({}));
    throw new Error(error.error || 'Request failed');
  }
  return response.json();
}

function showPanel(id) {
  panels.forEach(panel => panel.classList.toggle('active', panel.id === id));
}

navButtons.forEach(button => {
  button.addEventListener('click', () => {
    showPanel(button.dataset.target);
  });
});

async function loadSummary() {
  const summary = await fetchJson('/api/summary');
  summaryCards.innerHTML = `
    <div class="card"><h3>Donors</h3><p>${summary.donorsCount}</p></div>
    <div class="card"><h3>Pending Requests</h3><p>${summary.pendingRequests}</p></div>
    <div class="card"><h3>History Records</h3><p>${summary.historyCount}</p></div>
    <div class="card"><h3>Total Stock Groups</h3><p>${Object.keys(summary.stock).length}</p></div>
  `;
}

function makeButton(label, className) {
  const button = document.createElement('button');
  button.textContent = label;
  button.className = className;
  button.style = 'border:none; background:#dc2626; color:white; border-radius:0.75rem; padding:0.5rem 0.85rem; cursor:pointer;';
  return button;
}

function formatDate(dateString) {
  const date = new Date(dateString);
  if (isNaN(date)) return dateString;
  return date.toLocaleDateString();
}

async function loadDonors(query = '') {
  const url = query ? `/api/donors/search?q=${encodeURIComponent(query)}` : '/api/donors';
  const donors = await fetchJson(url);
  donorTable.innerHTML = donors.map(d => `
    <tr>
      <td>${d.name}</td>
      <td>${d.age}</td>
      <td>${d.bloodGroup}</td>
      <td>${d.city}</td>
      <td>${d.contact}</td>
      <td>${d.cnic || ''}</td>
      <td>${d.lastDonationDate}</td>
      <td>${d.disease || 'None'}</td>
      <td><button data-name="${encodeURIComponent(d.name)}">Delete</button></td>
    </tr>
  `).join('');
}

async function loadRequests() {
  const requests = await fetchJson('/api/requests');
  requestTable.innerHTML = requests.map(r => `
    <tr>
      <td>${r.patientName}</td>
      <td>${r.bloodGroup}</td>
      <td>${r.unitsNeeded}</td>
      <td>${r.hospital}</td>
      <td>${r.patientHealth || 'None'}</td>
      <td>${formatDate(r.createdAt)}</td>
    </tr>
  `).join('');
}

async function loadHistory() {
  const history = await fetchJson('/api/history');
  historyTable.innerHTML = history.map(record => `
    <tr>
      <td>${record.donorName}</td>
      <td>${record.cnic || ''}</td>
      <td>${record.bloodGroup}</td>
      <td>${formatDate(record.date)}</td>
    </tr>
  `).join('');
}

async function loadStock() {
  const stock = await fetchJson('/api/stock');
  stockTable.innerHTML = Object.keys(stock).map(bg => `
    <tr>
      <td>${bg}</td>
      <td>${stock[bg]}</td>
    </tr>
  `).join('');
}

async function loadBanks() {
  const data = await fetchJson('/api/banks');
  const banks = data.banks || [];
  const routes = data.routes || [];
  bankTable.innerHTML = banks.map(bank => `<tr><td>${bank.name}</td></tr>`).join('');
  routeTable.innerHTML = routes.map(route => `
    <tr>
      <td>${banks[route.from].name} ↔ ${banks[route.to].name}</td>
      <td>${route.distance}</td>
    </tr>
  `).join('');
  const bankSelect = document.getElementById('currentBank');
  if (bankSelect) {
    bankSelect.innerHTML = '<option value="">Select a Rawalpindi bank</option>' + banks.map(bank => `<option value="${bank.name}">${bank.name}</option>`).join('');
  }
  const reqHospitalSelect = document.getElementById('requestHospital');
  if (reqHospitalSelect) {
    reqHospitalSelect.innerHTML = '<option value="">Select Hospital</option>' + banks.map(bank => `<option value="${bank.name}">${bank.name}</option>`).join('');
  }
}

async function showMessage(message, isError = false) {
  if (messageBox) {
    messageBox.textContent = message;
    messageBox.style.borderColor = isError ? '#dc2626' : '#10b981';
    messageBox.style.color = isError ? '#7f1d1d' : '#064e3b';
    messageBox.style.background = isError ? '#fee2e2' : '#dcfce7';
    messageBox.style.display = 'block';
  }
}

async function initialize() {
  await loadSummary();
  await loadDonors();
  await loadRequests();
  await loadHistory();
  await loadStock();
  await loadBanks();
}

async function submitDonor(event) {
  event.preventDefault();
  const donor = {
    name: document.getElementById('donorName').value.trim(),
    age: Number(document.getElementById('donorAge').value),
    bloodGroup: document.getElementById('donorBlood').value.trim(),
    city: document.getElementById('donorCity').value.trim(),
    contact: document.getElementById('donorContact').value.trim(),
    cnic: document.getElementById('donorCnic').value.trim(),
    lastDonationDate: document.getElementById('donorDate').value,
    disease: document.getElementById('donorDisease').value.trim() || 'None'
  };
  try {
    await fetchJson('/api/donors', {
      method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify(donor)
    });
    donorForm.reset();
    await loadDonors();
    await loadSummary();
  } catch (err) {
    showMessage(err.message, true);
  }
}

async function submitRequest(event) {
  event.preventDefault();
  const request = {
    patientName: document.getElementById('requestPatient').value.trim(),
    bloodGroup: document.getElementById('requestBlood').value.trim(),
    unitsNeeded: Number(document.getElementById('requestUnits').value),
    hospital: document.getElementById('requestHospital').value.trim(),
    patientHealth: document.getElementById('requestPatientHealth').value.trim() || 'None'
  };
  try {
    await fetchJson('/api/requests', {
      method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify(request)
    });
    requestForm.reset();
    await loadRequests();
    await loadSummary();
    showMessage('Request created successfully.');
  } catch (err) {
    showMessage(err.message, true);
  }
}

async function processRequest() {
  try {
    await fetchJson('/api/requests/process', { method: 'POST' });
    await loadRequests();
    await loadStock();
    await loadSummary();
    showMessage('Processed next request successfully.');
  } catch (err) {
    showMessage(err.message, true);
  }
}

async function submitHistory(event) {
  event.preventDefault();
  const record = {
    donorName: document.getElementById('historyDonor').value.trim(),
    cnic: document.getElementById('historyCnic').value.trim(),
    bloodGroup: document.getElementById('historyBlood').value.trim(),
    date: document.getElementById('historyDate').value
  };
  if (!record.cnic) {
    return showMessage('Please provide donor CNIC for unique history tracking.', true);
  }
  try {
    await fetchJson('/api/history', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(record)
    });
    historyForm.reset();
    await loadHistory();
    await loadSummary();
    showMessage('Donation history recorded successfully.');
  } catch (err) {
    showMessage(err.message, true);
  }
}

async function updateStock() {
  const group = stockGroupInput.value.trim();
  if (!group) return showMessage('Select a blood group first.', true);
  const unitsStr = prompt('Enter units to add (use negative for decrease):', '0');
  if (unitsStr === null) return;
  const units = Number(unitsStr);
  if (!Number.isFinite(units)) return showMessage('Invalid units value.', true);
  try {
    await fetchJson('/api/stock/update', {
      method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify({ bloodGroup: group, units })
    });
    stockGroupInput.value = '';
    await loadStock();
    await loadSummary();
    showMessage('Stock updated successfully.');
  } catch (err) {
    showMessage(err.message, true);
  }
}

async function checkCompatibility() {
  const group = (stockGroupInput.value || '').trim();
  if (!group) {
    compatibleResult.textContent = '';
    return showMessage('Select a blood group to check compatibility.', true);
  }
  try {
    const compatible = await fetchJson(`/api/stock/compatibility/${encodeURIComponent(group)}`);
    if (!Array.isArray(compatible) || compatible.length === 0) {
      compatibleResult.textContent = 'No compatible donor groups found or invalid blood group.';
      compatibleResult.style.borderColor = '#dc2626';
    } else {
      compatibleResult.textContent = `Compatible donor groups: ${compatible.join(', ')}`;
      compatibleResult.style.borderColor = '#10b981';
    }
  } catch (err) {
    compatibleResult.textContent = '';
    showMessage(err.message, true);
  }
}

async function findClosestBank() {
  const bank = currentBankInput.value.trim();
  if (!bank) return showMessage('Select a bank from the dropdown.', true);
  try {
    const result = await fetchJson(`/api/banks/closest?bank=${encodeURIComponent(bank)}`);
    closestBankResult.textContent = `Closest bank to ${result.bank} is ${result.closest.bank} (${result.closest.distance} km)`;
    if (bankInfoBox) {
      bankInfoBox.textContent = `${result.bank} is closest to ${result.closest.bank} with an estimated distance of ${result.closest.distance} km.`;
    }
  } catch (err) {
    closestBankResult.textContent = err.message;
    if (bankInfoBox) {
      bankInfoBox.textContent = 'Unable to find nearest bank. Please try another bank.';
    }
  }
}

loadDonorsButton.addEventListener('click', () => loadDonors(donorSearch.value.trim()));
donorSearch.addEventListener('input', () => loadDonors(donorSearch.value.trim()));
donorForm.addEventListener('submit', submitDonor);
requestForm.addEventListener('submit', submitRequest);
historyForm.addEventListener('submit', submitHistory);
processRequestButton.addEventListener('click', processRequest);
updateStockButton.addEventListener('click', updateStock);
compatibilityCheck.addEventListener('click', checkCompatibility);
findClosest.addEventListener('click', findClosestBank);
donorTable.addEventListener('click', async event => {
  const button = event.target.closest('button[data-name]');
  if (!button) return;
  const name = decodeURIComponent(button.dataset.name);
  try {
    await fetchJson(`/api/donors/${encodeURIComponent(name)}`, { method: 'DELETE' });
    await loadDonors(donorSearch.value.trim());
    await loadSummary();
    showMessage(`Removed donor ${name}.`);
  } catch (err) {
    showMessage(err.message, true);
  }
});

initialize().catch(err => showMessage(err.message, true));
