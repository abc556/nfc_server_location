import React, {useState} from 'react';
import { useQRCode } from 'next-qrcode';
import './App.css';
import muselabsPng from './media/muselabs-crop.png'


const QRGenerator = () => {

    const [value, setValue] = useState<string>("");
    const { Canvas } = useQRCode();

    return (
        <div className="qrGeneratorContainer">
            <div className="appName">
                <img src={muselabsPng} className="navbarImg" />
                <div>QR-code Generator</div>
            </div>
          <input className="valueInput" value={value} onChange={e => setValue(e.target.value)} type={"text"} />
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
                // dark: '282c34',
                // dark: '4699db',
                light: '#FFBF60FF',
                // light: '#4699db'
                // light: '#7eb4e0'
              },
            }}
          />
        </div>
    )
}

export default QRGenerator;