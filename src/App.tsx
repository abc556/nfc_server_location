import React, {useState} from 'react';
import './App.css';
import muselabsPng from './media/muselabs.png'
import { useQRCode } from 'next-qrcode';

function App() {
  
  const { Canvas } = useQRCode();
  const [value, setValue] = useState("");

  return (
    <div className="App">
      <header className="App-header">
        <img src={muselabsPng} className="navbarImg" />
          <input className="valueInput" value={value} onChange={e => setValue(e.target.value)} type={"text"} />
        <div className="qrCodeContainer">
          <Canvas
            text={value!=""?value:"null"}
            options={{
              type: 'image/jpeg',
              quality: 0.3,
              level: 'M',
              margin: 3,
              scale: 4,
              width: 200,
              color: {
                dark: '#010599FF',
                light: '#FFBF60FF',
              },
            }}
          />
        </div>
      </header>
    </div>
  );
}

export default App;
