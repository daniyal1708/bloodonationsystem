# Blood Donation System

## Overview
A professional blood donation management system built in C++ with a polished console UI.

## Features
- Donor management
  - Add, delete, search, display, and sort donors
- Donation request management
  - Add new requests
  - Process requests with automatic blood stock consumption
  - View pending requests
  - Record donation history
  - View donation history
- Blood stock management
  - Display sorted stock levels
  - Update available units
  - Search stock by blood group
  - Compatible blood group checker for patient transfusions
- Blood bank network
  - Add sample blood banks
  - Display network routes
  - Find closest connected blood bank
- Data persistence
  - Donor data saved to `donors.txt`
  - Request queue saved to `requests.txt`
  - Donation history saved to `history.txt`
  - Blood stock saved to `blood_stock.txt`

## Usage
1. Open the solution `bloodonationsystem.sln` in Visual Studio.
2. Build the project in Debug or Release configuration.
3. Run the application.
4. Use the menu system to manage donors, requests, stock, and compatibility.

## Notes
- Blood group input is normalized automatically, so users can enter lowercase or spaced values.
- The request processor will only dequeue a request when sufficient stock is available.
- The compatibility checker displays valid donor groups for a patient blood group.

## Files
- `bloodonationsystem/BloodDonation.cpp` - main implementation
- `bloodonationsystem/BloodDonation.h` - class and function declarations
- `bloodonationsystem.sln` - Visual Studio solution
- `donors.txt`, `requests.txt`, `history.txt`, `blood_stock.txt` - persisted data files

## Web Application
A web version of the blood donation system is available in the `webapp` folder.

### Run in Visual Studio Code
1. Open the workspace in VS Code.
2. Run the task `Install Web Dependencies` from the Terminal > Run Task menu.
3. Run the task `Run Blood Donation Web App`.
4. Open http://localhost:3000 in your browser.

### Website Files
- `webapp/server.js` - Express backend and JSON data persistence
- `webapp/package.json` - web project dependencies and scripts
- `webapp/public/index.html` - web UI
- `webapp/public/styles.css` - website styling
- `webapp/public/app.js` - frontend interactivity and API calls
