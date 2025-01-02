import React, { useState, useEffect } from "react";
import "./styles.css";
// ToDo: how to mount the config.js or config.json file into the docker container and load it into the script at runtime?

  function Settings() {
    const [categories, setCategories] = useState([{ category: "", indicators: [""] }]);

    useEffect(() => {
      // Fetch settings from the backend
      const fetchSettings = async () => {
        try {
          const config = await fetch('/frontendConfig.json').then(response => response.json());
          const targetURL = `${config.serverAddress}/settings`;
          console.log(`sending GET to: ${targetURL}`);
          const response = await fetch(`${targetURL}`, {
            method: "GET",
          });
          if (response.ok) {
            const data = await response.json();
            // Populate categories with backend data
            const loadedCategories = Object.entries(data).map(([key, value]) => ({
              category: key,
              indicators: value.length ? value : [""],
            }));
            setCategories(loadedCategories);
          } else {
            console.error("Failed to fetch settings from the backend.");
          }
        } catch (error) {
          console.error("Error fetching settings:", error);
        }
      };

      fetchSettings();
  }, []);

  const handleCategoryChange = (index, value) => {
    const updatedCategories = [...categories];
    updatedCategories[index].category = value;
    setCategories(updatedCategories);
  };

  const handleIndicatorChange = (lineIndex, indicatorIndex, value) => {
    const updatedCategories = [...categories];
    const indicators = updatedCategories[lineIndex].indicators;

    // Update the current indicator's value
    indicators[indicatorIndex] = value;

    // Add a new empty indicator if the last one is filled
    if (indicatorIndex === indicators.length - 1 && value !== "") {
        indicators.push("");
    }

    // Ensure only the last empty indicator persists
    const nonEmptyIndicators = indicators.filter((indicator) => indicator !== "");
    if (indicators.length - nonEmptyIndicators.length > 1) {
        updatedCategories[lineIndex].indicators = [
            ...nonEmptyIndicators,
            ""
        ]; // Keep only one empty indicator
    }

    setCategories(updatedCategories);
  };

  const handleAddCategory = () => {
    setCategories([...categories, { category: "", indicators: [""] }]);
  };

  const handleRemoveCategory = (lineIndex) => {
    if (categories.length > 1) {
      const updatedCategories = categories.filter((_, index) => index !== lineIndex);
      setCategories(updatedCategories);
    } else {
      alert("At least one category must remain.");
    }
  };

  const handleApply = async () => {
    const payload = categories.reduce((acc, curr) => {
      if (curr.category) {
        acc[curr.category] = curr.indicators.filter((i) => i !== "");
      }
      return acc;
      
    }, {});
    
    try {
      const config = await fetch('/frontendConfig.json').then(response => response.json());
      const hostAddress = `${config.serverAddress}/settings`;
      const response = await fetch(`${hostAddress}`, {
      method: 'POST',
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(payload),
    });
    if (response.ok) {
      alert("Settings saved successfully!");
    } else {
      alert("Failed to save settings.");
    }
  } catch (error) {
    console.error("Error saving settings:", error);
  }
  };

  return (
    <div style={{ padding: "20px" }}>
      <h1>Settings Page</h1>
      <h2>(functionality not implemented yet)</h2>
      {categories.map((category, lineIndex) => (
        <div key={lineIndex} style={{ marginBottom: "10px", display: "flex", alignItems: "center" }}>
          <button className="button"
            onClick={() => handleRemoveCategory(lineIndex)}
            style={{
              backgroundColor: "red"
            }}
            onMouseEnter={(e) => (e.target.style.backgroundColor = "darkred")}
            onMouseLeave={(e) => (e.target.style.backgroundColor = "red")}
          >
            -
          </button>
          <input className="text-input"
            type="text"
            placeholder="Category"
            value={category.category}
            onChange={(e) => handleCategoryChange(lineIndex, e.target.value)}
            style={{ marginRight: "10px" }}
          />
          {category.indicators.map((indicator, indicatorIndex) => (
            <input className="text-input"
              key={indicatorIndex}
              type="text"
              placeholder="Indicator"
              value={indicator}
              onChange={(e) => handleIndicatorChange(lineIndex, indicatorIndex, e.target.value)}
              style={{ marginRight: "10px", backgroundColor: "#fff3b0" }}
              onMouseEnter={(e) => (e.target.style.backgroundColor = "#fcd34d")}
              onMouseLeave={(e) => (e.target.style.backgroundColor = "#fff3b0")}
            />
          ))}
        </div>
      ))}
      <button className="button"
        onClick={() => handleAddCategory()}>
          +
      </button>
      <div style={{ marginTop: "20px" }}>
        <button onClick={handleApply}>Apply</button>
      </div>
    </div>
  );
}

export default Settings;