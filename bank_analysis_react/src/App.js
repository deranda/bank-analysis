
import React from 'react';
import { BrowserRouter as Router, Routes, Route } from 'react-router-dom';
import FileUploadWithDropdown from "./pages/FileUploadWithDropdown.js";
import Settings from "./pages/pageSettings.js"
import Navbar from './components/Navbar';

function App() {
    return (
        <Router>
            <Navbar />
            <Routes>
            <Route path="/" element={<FileUploadWithDropdown />} />
            <Route path="/settings" element={<Settings />} />
            </Routes>
      </Router>
    );
}

export default App;
