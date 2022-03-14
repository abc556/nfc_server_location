import React, {useState} from 'react';
import logo from './logo.svg';
import './App.css';
// import { useQRCode } from 'react-qrcodes';
import QRCode from "react-qr-code"
import muselabsPng from './media/muselabs.png'

function App() {
  
  const [value, setValue] = useState("");

  return (
    <div className="App">
      <header className="App-header">
        <img src={muselabsPng} className="navbarImg" />
        <input className="valueInput" value={value} onChange={e => setValue(e.target.value)} type={"text"} />
        <QRCode value={value} />
      </header>
    </div>
  );
}

export default App;
