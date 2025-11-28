/*
 * Organizational Hierarchy Viewer - JavaScript
 * 
 * Sample CSV format (place at data/employees.csv):
 * id,name,title,manager_id
 * 1,John Doe,CEO,
 * 2,Jane Smith,VP,1
 * 3,Arun Kumar,Director,2
 * 
 * Features:
 * - Robust CSV parsing with whitespace/blank line handling
 * - Circular reference detection
 * - Debounced search (200ms)
 * - Keyboard-navigable suggestions
 * - Collapsible tree visualization
 */

const CSV_PATH = '/employees.csv';

let employees = [];
let employeeMap = new Map();
let rootEmployees = [];
let selectedEmployee = null;
let isCompactView = false;
let searchTimeout = null;
let activeSuggestionIndex = -1;

const elements = {
    searchInput: document.getElementById('search-input'),
    idInput: document.getElementById('id-input'),
    idSearchBtn: document.getElementById('id-search-btn'),
    suggestions: document.getElementById('search-suggestions'),
    employeeDetails: document.getElementById('employee-details'),
    emptyState: document.getElementById('empty-state'),
    errorMessage: document.getElementById('error-message'),
    loadingMessage: document.getElementById('loading-message'),
    employeeName: document.getElementById('employee-name'),
    employeeTitle: document.getElementById('employee-title'),
    employeeId: document.getElementById('employee-id'),
    employeeEmail: document.getElementById('employee-email'),
    employeePhone: document.getElementById('employee-phone'),
    reportingChain: document.getElementById('reporting-chain'),
    peersList: document.getElementById('peers-list'),
    directReports: document.getElementById('direct-reports')
};

class Employee {
    constructor(id, name, title, managerId = null, email = '', phone = '') {
        this.id = id;
        this.name = name ? name.trim() : '';
        this.title = title ? title.trim() : '';
        this.email = email ? email.trim() : '';
        this.phone = phone ? phone.trim() : '';
        this.managerId = managerId;
        this.manager = null;
        this.children = [];
        this.hasCircularRef = false;
    }
}

function trim(str) {
    return str ? str.trim() : '';
}

function parseCSV(csvText) {
    const lines = csvText.split('\n');
    const parsedEmployees = [];
    let isFirstLine = true;
    
    for (let i = 0; i < lines.length; i++) {
        const line = trim(lines[i]);
        
        if (!line || line.length === 0) {
            continue;
        }
        
        const parts = line.split(',').map(p => trim(p));
        
        if (parts.length < 3) {
            continue;
        }
        
        const id = parseInt(parts[0]);
        const name = parts[1];
        const title = parts[2];
        const managerIdStr = parts[3] || '';
        const email = parts[4] || '';
        const phone = parts[5] || '';
        
        
        if (isNaN(id) || !name || !title) {
            if (isFirstLine && name.toLowerCase().includes('name')) {
                isFirstLine = false;
                continue;
            }
            continue;
        }
        
        isFirstLine = false;
        
        let managerId = null;
        if (managerIdStr && managerIdStr !== '-1' && managerIdStr !== '0' && managerIdStr !== '') {
            const parsedManagerId = parseInt(managerIdStr);
            if (!isNaN(parsedManagerId) && parsedManagerId > 0) {
                managerId = parsedManagerId;
            }
        }
        
        parsedEmployees.push(new Employee(id, name, title, managerId, email, phone));

    }
    
    return parsedEmployees;
}

function detectCircularReference(employee, ancestors = new Set()) {
    if (ancestors.has(employee.id)) {
        employee.hasCircularRef = true;
        return employee.id;
    }
    
    if (!employee.manager) {
        return null;
    }
    
    const newAncestors = new Set(ancestors);
    newAncestors.add(employee.id);
    
    const cycleStartId = detectCircularReference(employee.manager, newAncestors);
    
    if (cycleStartId !== null) {
        employee.hasCircularRef = true;
        
        if (employee.id === cycleStartId) {
            return null;
        }
        return cycleStartId;
    }
    
    return null;
}

function buildHierarchy() {
    employeeMap.clear();
    rootEmployees = [];
    
    employees.forEach(emp => {
        employeeMap.set(emp.id, emp);
        emp.children = [];
        emp.manager = null;
        emp.hasCircularRef = false;
    });
    
    employees.forEach(emp => {
        if (emp.managerId !== null) {
            const manager = employeeMap.get(emp.managerId);
            if (manager) {
                emp.manager = manager;
                manager.children.push(emp);
            }
        }
    });
    
    employees.forEach(emp => {
        if (emp.manager === null) {
            rootEmployees.push(emp);
        }
    });
    
    employees.forEach(emp => {
        detectCircularReference(emp);
    });
}

async function loadEmployeeData() {
    try {
        elements.loadingMessage.classList.remove('hide');
        elements.errorMessage.classList.remove('show');
        
        const response = await fetch(CSV_PATH);
        
        if (!response.ok) {
                throw new Error(`Failed to load CSV file: ${response.statusText}. Ensure 'employees.csv' exists in the public folder.`);
        }
        
        const csvText = await response.text();
        employees = parseCSV(csvText);
        
        if (employees.length === 0) {
            throw new Error('No valid employee data found in CSV file');
        }
        
        buildHierarchy();
        
        elements.loadingMessage.classList.add('hide');
        console.log(`Loaded ${employees.length} employees`);
        
    } catch (error) {
        elements.loadingMessage.classList.add('hide');
        showError(`Error loading employee data: ${error.message}`);
        console.error('Load error:', error);
    }
}

function showError(message) {
    elements.errorMessage.textContent = message;
    elements.errorMessage.classList.add('show');
    setTimeout(() => {
        elements.errorMessage.classList.remove('show');
    }, 5000);
}

function debounce(func, wait) {
    return function(...args) {
        clearTimeout(searchTimeout);
        searchTimeout = setTimeout(() => func.apply(this, args), wait);
    };
}

function searchEmployees(query) {
    if (!query || query.trim() === '') return [];
    const lowerQuery = query.toLowerCase();

    return employees.filter(emp => {
    const name = emp.name || '';
    const title = emp.title || '';
    const email = emp.email || '';
    const phone = emp.phone || '';

    return name.toLowerCase().includes(lowerQuery) ||
           title.toLowerCase().includes(lowerQuery) ||
           email.toLowerCase().includes(lowerQuery) ||
           phone.toLowerCase().includes(lowerQuery) ||
           String(emp.id) === query.trim();
});

}

function showSuggestions(results) {
    elements.suggestions.innerHTML = '';
    activeSuggestionIndex = -1;
    elements.searchInput.removeAttribute('aria-activedescendant');
    
    if (results.length === 0) {
        elements.suggestions.innerHTML = '<div class="no-results">No results found</div>';
        elements.suggestions.classList.add('show');
        return;
    }
    
    results.forEach((emp, index) => {
        const item = document.createElement('div');
        item.className = 'suggestion-item';
        item.setAttribute('role', 'option');
        item.setAttribute('id', `suggestion-${index}`);
        item.setAttribute('data-index', index);
        item.setAttribute('data-emp-id', emp.id);
        
        item.innerHTML = `
            <h4>${emp.name}</h4>
            <p>${emp.title}</p>
            <div class="suggest-meta">
                <span>ID: ${emp.id}</span>
                ${emp.email ? `<span>• ${emp.email}</span>` : ''}
                ${emp.phone ? `<span>• ${emp.phone}</span>` : ''}
            </div>
        `;
        
        item.addEventListener('click', () => selectEmployee(emp.id));
        elements.suggestions.appendChild(item);
    });
    
    elements.suggestions.classList.add('show');
}

function hideSuggestions() {
    elements.suggestions.classList.remove('show');
    activeSuggestionIndex = -1;
    elements.searchInput.removeAttribute('aria-activedescendant');
}

function handleSearchInput() {
    const query = elements.searchInput.value;
    
    if (!query || query.trim() === '') {
        hideSuggestions();
        return;
    }
    
    const results = searchEmployees(query);
    showSuggestions(results);
}

function selectEmployee(empId) {
    const emp = employeeMap.get(empId);
    
    if (!emp) {
        showError('Employee not found');
        return;
    }
    
    selectedEmployee = emp;
    hideSuggestions();
    displayEmployeeDetails(emp);
    
    elements.emptyState.style.display = 'none';
    elements.employeeDetails.style.display = 'block';
    
    elements.employeeDetails.scrollIntoView({ behavior: 'smooth', block: 'start' });
}

function displayEmployeeDetails(emp) {
    elements.employeeName.textContent = emp.name;
    elements.employeeTitle.textContent = emp.title;
    elements.employeeEmail.textContent = emp.email;
    elements.employeePhone.textContent = emp.phone;
    
    elements.employeeId.querySelector('span').textContent = emp.id;
    
    displayReportingChain(emp);
    displayPeers(emp);
    displayDirectReports(emp);
}

function displayReportingChain(emp) {
    elements.reportingChain.innerHTML = '';
    
    const chain = [];
    let current = emp;
    
    while (current) {
        chain.unshift(current);
        current = current.manager;
    }
    
    if (chain.length === 1) {
        elements.reportingChain.innerHTML = '<div class="no-results">Top-level employee (no manager)</div>';
        return; 
    }
    
    chain.forEach(person => {
        const item = document.createElement('div');
        item.className = 'chain-item';
        
        if (person.id === emp.id) {
            item.classList.add('selected');
        }
        
        item.innerHTML = `
            <h4>${person.name}</h4>
            <p>${person.title}</p>
            <div class="suggest-meta">
                <span>ID: ${person.id}</span>
                ${person.email ? `<span>• ${person.email}</span>` : ''}
                ${person.phone ? `<span>• ${person.phone}</span>` : ''}
            </div>
        `;
        
        elements.reportingChain.appendChild(item);
    });
}

function displayPeers(emp) {
    elements.peersList.innerHTML = '';
    
    if (!emp.manager) {
        elements.peersList.innerHTML = '<div class="no-results">No peers (top-level employee)</div>';
        return;
    }
    
    const peers = emp.manager.children.filter(child => child.id !== emp.id);
    
    if (peers.length === 0) {
        elements.peersList.innerHTML = '<div class="no-results">No peers found</div>';
        return;
    }
    
    peers.forEach(peer => {
        const item = document.createElement('div');
        item.className = 'peer-item';
        item.setAttribute('tabindex', '0');
        item.setAttribute('role', 'button');
        item.setAttribute('aria-label', `View details for ${peer.name}`);
        
        item.innerHTML = `
            <h4>${peer.name}</h4>
            <p>${peer.title}</p>
            <div class="suggest-meta">
                <span>ID: ${peer.id}</span>
                ${peer.email ? `<span>• ${peer.email}</span>` : ''}
                ${peer.phone ? `<span>• ${peer.phone}</span>` : ''}
            </div>
        `;
        
        item.addEventListener('click', () => selectEmployee(peer.id));
        item.addEventListener('keypress', (e) => {
            if (e.key === 'Enter') {
                selectEmployee(peer.id);
            }
        });
        
        elements.peersList.appendChild(item);
    });
}

function displayDirectReports(emp) {
    elements.directReports.innerHTML = '';
    
    if (emp.children.length === 0) {
        elements.directReports.innerHTML = '<div class="no-results">No direct reports</div>';
        return;
    }
    
    emp.children.forEach(child => {
        renderTreeNode(child, elements.directReports, 0);
    });
}

function renderTreeNode(emp, container, depth) {
    const nodeDiv = document.createElement('div');
    nodeDiv.className = isCompactView ? 'tree-node compact' : 'tree-node';
    
    const itemDiv = document.createElement('div');
    itemDiv.className = 'tree-item';
    itemDiv.setAttribute('tabindex', '0');
    itemDiv.setAttribute('role', 'button');
    itemDiv.setAttribute('aria-label', `${emp.name}, ${emp.title}`);
    
    if (emp.children.length > 0) {
        itemDiv.classList.add('has-children');
        itemDiv.setAttribute('aria-expanded', 'false');
    }
    
    const errorBadge = emp.hasCircularRef ? '<span class="error-badge">⚠ Circular Ref</span>' : '';
    
    const expandIcon = emp.children.length > 0 ? '<span class="expand-icon">▶</span>' : '';
    
    itemDiv.innerHTML = `
        ${expandIcon}
        <div class="tree-item-content">
            <h4>${emp.name} ${errorBadge}</h4>
            <p>${emp.title}</p>
            <div class="suggest-meta">
                <span>ID: ${emp.id}</span>
                ${emp.email ? `<span>• ${emp.email}</span>` : ''}
                ${emp.phone ? `<span>• ${emp.phone}</span>` : ''}
            </div>
        </div>
    `;
    
    const childrenContainer = document.createElement('div');
    childrenContainer.className = 'tree-children';
    childrenContainer.style.display = 'none';
    
    if (emp.children.length > 0) {
        emp.children.forEach(child => {
            renderTreeNode(child, childrenContainer, depth + 1);
        });
        
        const expandIconEl = itemDiv.querySelector('.expand-icon');
        const contentEl = itemDiv.querySelector('.tree-item-content');
        
        if (expandIconEl) {
            expandIconEl.addEventListener('click', (e) => {
                e.stopPropagation();
                toggleNode(itemDiv, childrenContainer);
            });
        }
        
        if (contentEl) {
            contentEl.addEventListener('click', (e) => {
                e.stopPropagation();
                selectEmployee(emp.id);
            });
        }
        
        itemDiv.addEventListener('keypress', (e) => {
            if (e.key === 'Enter') {
                e.preventDefault();
                toggleNode(itemDiv, childrenContainer);
            } else if (e.key === ' ') {
                e.preventDefault();
                selectEmployee(emp.id);
            }
        });
    } else {
        itemDiv.addEventListener('click', () => selectEmployee(emp.id));
        itemDiv.addEventListener('keypress', (e) => {
            if (e.key === 'Enter' || e.key === ' ') {
                e.preventDefault();
                selectEmployee(emp.id);
            }
        });
    }
    
    nodeDiv.appendChild(itemDiv);
    nodeDiv.appendChild(childrenContainer);
    container.appendChild(nodeDiv);
}

function toggleNode(itemDiv, childrenContainer) {
    const isExpanded = childrenContainer.style.display !== 'none';
    
    if (isExpanded) {
        childrenContainer.style.display = 'none';
        itemDiv.classList.remove('expanded');
        itemDiv.setAttribute('aria-expanded', 'false');
    } else {
        childrenContainer.style.display = 'block';
        itemDiv.classList.add('expanded');
        itemDiv.setAttribute('aria-expanded', 'true');
    }
}

function handleKeyboardNavigation(e) {
    const suggestions = elements.suggestions.querySelectorAll('.suggestion-item');
    
    if (suggestions.length === 0) return;
    
    if (e.key === 'ArrowDown') {
        e.preventDefault();
        activeSuggestionIndex = Math.min(activeSuggestionIndex + 1, suggestions.length - 1);
        updateActiveSuggestion(suggestions);
    } else if (e.key === 'ArrowUp') {
        e.preventDefault();
        activeSuggestionIndex = Math.max(activeSuggestionIndex - 1, 0);
        updateActiveSuggestion(suggestions);
    } else if (e.key === 'Enter' && activeSuggestionIndex >= 0) {
        e.preventDefault();
        const empId = parseInt(suggestions[activeSuggestionIndex].getAttribute('data-emp-id'));
        selectEmployee(empId);
    } else if (e.key === 'Escape') {
        hideSuggestions();
    }
}

function updateActiveSuggestion(suggestions) {
    suggestions.forEach((item, index) => {
        if (index === activeSuggestionIndex) {
            item.classList.add('active');
            item.scrollIntoView({ block: 'nearest', behavior: 'smooth' });
            elements.searchInput.setAttribute('aria-activedescendant', item.id);
        } else {
            item.classList.remove('active');
        }
    });
}



function searchById() {
    const id = parseInt(elements.idInput.value);
    
    if (isNaN(id)) {
        showError('Please enter a valid employee ID');
        return;
    }
    
    const emp = employeeMap.get(id);
    
    if (!emp) {
        showError(`Employee with ID ${id} not found`);
        return;
    }
    
    selectEmployee(id);
    elements.idInput.value = '';
}

elements.searchInput.addEventListener('input', debounce(handleSearchInput, 200));
elements.searchInput.addEventListener('keydown', handleKeyboardNavigation);

elements.idInput.addEventListener('keypress', (e) => {
    if (e.key === 'Enter') {
        searchById();
    }
});

elements.idSearchBtn.addEventListener('click', searchById);



document.addEventListener('click', (e) => {
    if (!elements.suggestions.contains(e.target) && e.target !== elements.searchInput) {
        hideSuggestions();
    }
});

loadEmployeeData();