import React, { useState, useEffect } from "react";


const FileUploadWithDropdown = () => {
    const [selectedBank, setSelectedBank] = useState("GLS"); // Default selected value
    const [csvFile, setCsvFile] = useState(null);
    const [bankPositions, setBankPositions] = useState(null);

    useEffect(() => {
        // Load data from localStorage on initial render
        const savedPositions = JSON.parse(localStorage.getItem('bankPositions'));
        if (savedPositions) {
            setBankPositions(savedPositions);
        }
        const savedCsvSelection = localStorage.getItem('csvFile');
        if (savedCsvSelection) {
            setCsvFile(savedCsvSelection);
        }
    }, []);

    const handleFileChange = (event) => {
        const selectedFile = event.target.files[0];
        if (selectedFile) {
            setCsvFile(selectedFile);
            localStorage.setItem('csvFile', selectedFile); // Persist file name
        }
    };

    const handleBankChange = (event) => {
        setSelectedBank(event.target.value);
    };

    const handleSubmit = async (event) => {
        event.preventDefault();
        if (!csvFile) {
            alert("Please select a file before uploading!");
            return;
        }

        const formData = new FormData();
        formData.append('csvFile', csvFile);
        formData.append('bank', selectedBank);
        try {
            const config = await fetch('/frontendConfig.json').then(response => response.json());
            const hostAddress = `${config.serverAddress}/upload`;
            const response = await fetch(`${hostAddress}`, {
            method: 'POST',
            body: formData,
          });
          const bankPositions = await response.json();
          setBankPositions(bankPositions);
        // Save to localStorage
        localStorage.setItem('bankPositions', JSON.stringify(bankPositions));
        console.log('File sent:', bankPositions);
        } catch (error) {
          console.error('Error sending file:', error);
        }
    };


    return (
        <div>
            <h1>Bank Analysis</h1>
            <form onSubmit={handleSubmit} style={{ display: "flex", gap: "10px" }}>
                <div>
                    <label htmlFor="fileInput">Upload CSV:</label>
                    <input
                        id="fileInput"
                        type="file"
                        accept=".csv"
                        onChange={handleFileChange}
                    />
                </div>
                <div>
                    <label htmlFor="bankSelect">Bank:</label>
                    <select id="bankSelect" value={selectedBank} onChange={handleBankChange}>
                        <option value="GLS">GLS</option>
                        <option value="DKB">DKB</option>
                    </select>
                </div>
                <button type="submit">Upload</button>
            </form>

            {bankPositions && bankPositions.transactions && bankPositions.transactions.length > 0 ? (
                <table>
                    <thead>
                        <tr>
                            <th>Datum</th>
                            <th>Betrag</th>
                            <th>Beschreibung</th>
                            <th>Kategorie</th>
                        </tr>
                    </thead>
                    <tbody>
                        {bankPositions.transactions.map((row, index) => (
                            <tr key={index}>
                                <td>{row.Datum}</td>
                                <td>{row.Betrag}</td>
                                <td>{row.Beschreibung}</td>
                                <td>{row.Kategorie}</td>
                            </tr>
                        ))}
                    </tbody>
                </table>
            ) : (
                <p>No transactions to display</p>
            )}
        </div>
    );
};

export default FileUploadWithDropdown;