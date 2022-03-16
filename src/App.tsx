import React, {useCallback, useState, useRef, useEffect, useMemo} from 'react';
import './App.css';
import { useQRCode } from 'next-qrcode';
import Papa from 'papaparse';
import jschardet from 'jschardet';
import CSVparse from './CSVparse'
import QRGenerator from './QRGenerator';
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome'
import {faUpload, faCloudUploadAlt, faCheck,faBan, faArrowCircleLeft, faArrowCircleRight, faHourglassStart,faPlay, faStop } from '@fortawesome/fontawesome-free-solid'
// import 'font-awesome/css/font-awesome.min.css';
import {buildStyles, CircularProgressbar, CircularProgressbarWithChildren} from "react-circular-progressbar"
import "react-circular-progressbar/dist/styles.css"
import {FileUploader} from "react-drag-drop-files"
import {useDropzone} from 'react-dropzone'
import StyledDropzone from './StyledDropzone';

const INITIAL_COLOR = "223,223,223"
const FINAL_COLOR = "158,255,255"
const FINAL_COLOR_HEX = "#9EFFFF"
const FAILED_COLOR = "252,180,180"
const FAILED_COLOR_HEX = "#FFB4B4"

export interface IVaccine {
  IDtag: string;
  Count: number;
  DateTime: string;
  rawInfo: string;
}

const baseStyle = {
  // flex: 1,
  display: 'flex',
  flexDirection: 'column',
  alignItems: 'center',
  padding: '20px',
  borderWidth: 2,
  borderRadius: 2,
  borderColor: '#eeeeee',
  borderStyle: 'dashed',
  backgroundColor: '#fafafa',
  color: '#bdbdbd',
  outline: 'none',
  transition: 'border .24s ease-in-out'
};

const focusedStyle = {
  borderColor: '#2196f3'
};

const acceptStyle = {
  borderColor: '#00e676'
};

const rejectStyle = {
  borderColor: '#ff1744'
};

const countdownRgbGradient = (currentMsec:number, countdownMsec:number, initialRGB:string, endingRGB:string, isHex=false) => {
  let initialRGBarray = initialRGB.split(',');
  let endingRGBarray = endingRGB.split(',');
  let redSlope = (parseInt(endingRGBarray[0])-parseInt(initialRGBarray[0]))/(0-countdownMsec);
  let greenSlope = (parseInt(endingRGBarray[1])-parseInt(initialRGBarray[1]))/(0-countdownMsec);
  let blueSlope = (parseInt(endingRGBarray[2])-parseInt(initialRGBarray[2]))/(0-countdownMsec);
  let currentRed = Math.floor(redSlope*(currentMsec-countdownMsec)+parseInt(initialRGBarray[0]))
  let currentGreen = Math.floor(greenSlope*(currentMsec-countdownMsec)+parseInt(initialRGBarray[1]))
  let currentBlue = Math.floor(blueSlope*(currentMsec-countdownMsec)+parseInt(initialRGBarray[2]))
  if(isHex){
    return '#'+currentRed.toString(16)+currentGreen.toString(16)+currentBlue.toString(16)
  }
  return `${currentRed},${currentGreen},${currentBlue}`
}

function App() {

  let checkEncoding = (base64Str:any) => {
    const str = atob(base64Str.split(";base64,")[1]); 
    let encoding = jschardet.detect(str);
    let encodingString = encoding.encoding;
    encodingString = "ASCII";
    console.log(encodingString)
    return encodingString;
  }

  const onDrop = useCallback((files) => {
    files.forEach((file:any) => {
      readCSV(file);
    })
  },[])

  const {getRootProps, getInputProps, isFocused, isDragAccept, isDragReject} = useDropzone({onDrop})
  const style = useMemo(() => ({
    ...baseStyle,
    ...(isFocused ? focusedStyle : {}),
    ...(isDragAccept ? acceptStyle : {}),
    ...(isDragReject ? rejectStyle : {})
  }),[
    isFocused,
    isDragAccept,
    isDragReject
  ])

  const { Canvas } = useQRCode();
  const [dragging, setDragging] = useState<boolean>(false);
  const [uploaded, setUploaded] = useState<boolean>(false);
  const [vaccines, setVaccines] = useState<IVaccine[]>([]);
  const [vaccineIndex, setVaccineIndex] = useState<number>(0);
  const [vaccineStatus, setVaccineStatus] = useState<any>({});
  const [countdownSec, setCountdownSec] = useState<number>(4);
  const [milliseconds, setMilliseconds] = useState<number>(300);
  const [runningStatus, setRunningStatus] = useState<number>(0);

  const intervalRef:any = useRef(null);
  const dropRef:any = useRef(null);
  const uploadRef:any = useRef(null);

  let vaccineIdx = vaccineIndex
  let vaccineArraylength = vaccines.length
  let ms = milliseconds

  useEffect(() => {
    const div = dropRef.current
    div.addEventListener('dragenter', (event:any) => {
      event.preventDefault();
      setDragging(true);
      console.log('drag enter');
    }, false)
    div.addEventListener('dragleave', (event:any) => {
      event.preventDefault();
      setDragging(false);
      console.log('drag leave');
    })
    div.addEventListener('drop', (event:any) => {
      event.preventDefault();
      setDragging(false);
      console.log('drop');
      let files = event.dataTransfer.files
      readCSV(files);
    }, false)
  },[])

  useEffect(() => {
    if(ms<=0){
      if(vaccineIdx+1<vaccineArraylength){
        reset();
        setVaccineIndex((index) => index + 1)
        start();
      }else{
        stop()
      }
      setVaccineStatus((status:object) => {
        return {...status, [vaccineIdx+1]:true}
      })
    }
  }, [ms])

  const countdown = () => {
    setMilliseconds((milliseconds) => milliseconds - 1)
  }

  const start = () => {
    setRunningStatus(1);
    clearInterval(intervalRef.current);
    intervalRef.current = setInterval(countdown, 10);
  }

  const stop = () => {
    setRunningStatus(2);
    clearInterval(intervalRef.current);
  }

  const reset = () => {
    setRunningStatus(0);
    clearInterval(intervalRef.current);
    setMilliseconds((countdownSec)*100);
  }

  const scanSuccess = () => {
    setVaccineStatus((status:object) => {
      return {...status, [vaccineIdx+1]:true}
    })
    reset();
    setVaccineIndex((index) => index + 1)
  }

  const scanFailed = () => {
    setVaccineStatus((status:object) => {
      return {...status, [vaccineIdx+1]:false}
    })
    reset();
    setVaccineIndex((index) => index + 1)
  }

  const readCSV = (files:any) =>{
    // let file = event.target.files[0]
    try{
      let file=files[0];
      let fileReader = new FileReader();
      fileReader.readAsDataURL(file);
      fileReader.onload = function(event){
        const data = event?.target?.result;
        const encoding = checkEncoding(data);
        Papa.parse(file, {
          encoding: encoding,
          complete: function(results){
            const res = results.data;
            if(res[res.length -1] === ""){
              res.pop();
            }
            let column:any = {}
            let tempVaccines:IVaccine[] = []
            res.map((row:any,index) => {
              if(index==0){
                row.map((colName:string, index:number) => {
                  switch(colName){
                    case 'IDtag':
                      column['IDtag'] = index;
                      break;
                    case 'Count':
                      column['Count'] = index;
                      break;
                    case 'DateTime':
                      column['DateTime'] = index;
                      break;
                    case 'Rawinfo':
                      column['RawInfo'] = index;
                      break;
                  }
                })
              }else{
                tempVaccines.push({
                  IDtag: row[column['IDtag']],
                  Count: row[column['Count']],
                  DateTime: row[column['DateTime']],
                  rawInfo: row[column['RawInfo']]
                })
              }
            })
            setVaccines(tempVaccines);
          }
        })
      }
      setUploaded(true)
    }catch(e){
      console.log('error')
      setUploaded(false)
    }
  }

  return (
    <div className="App">
      <header className="App-header">
        <div className="outerContainer">
          <QRGenerator />
          {!uploaded? 
          <div className={`uploadPanel ${dragging && "dragging"}`} ref={dropRef} >
            <FontAwesomeIcon style={{height:150}} icon={faCloudUploadAlt} />
            <div style={{margin:20}}>
              <div>Drag Here</div>
              <div>(.csv file in UTF-8)</div>
            </div>
            <label className="upload" htmlFor="custom-file-input">Upload</label>
            <input ref={uploadRef} className="custom-file-input" type="file" accept=".csv" onChange={event => readCSV(event.target.files)} />
          </div> 
          :
          <>
          <div className="qrCSVContainer"
            style={vaccineStatus[vaccineIndex+1]?
              {background:`rgba(${FINAL_COLOR},1)`}
              :vaccineStatus[vaccineIndex+1]==false?
              {background: `rgba(${FAILED_COLOR},1)`}
              :{background:`rgba(${countdownRgbGradient(milliseconds, countdownSec*100, INITIAL_COLOR, FINAL_COLOR)},1)`}}>
            {/* {vaccines.length>=2? */}
              <>
                <div className="vaccineQrInfo">
                  {vaccines.length>=2?
                    <>
                      <div>No. {vaccineIndex+1}</div>
                      <div>
                        <div>{vaccines[vaccineIndex]['IDtag']}</div>
                        <div>{vaccines[vaccineIndex]['DateTime']}</div>
                      </div>
                      <div>{vaccineIndex+1}/{vaccines.length}</div>
                    </>:
                    <>Pending Upload</>
                  }
                </div>
                {vaccines.length>=2?
                <Canvas
                  text={vaccines[vaccineIndex]['rawInfo']}
                  options={{
                    type: 'image/jpeg',
                    quality: 0.3,
                    level: 'M',
                    margin: 3,
                    scale: 4,
                    width: 330,
                    color: {
                      dark: '#8f5621',
                      light:vaccineStatus[vaccineIndex+1]?FINAL_COLOR_HEX:vaccineStatus[vaccineIndex+1]==false?FAILED_COLOR_HEX:countdownRgbGradient(milliseconds, countdownSec*100, INITIAL_COLOR, FINAL_COLOR, true)
                    },
                  }}
                />
                :
                <div ref={dropRef} >
                  <FontAwesomeIcon icon={faUpload} />
                  Drag Here 
                </div>
                }
                <div className="buttonContainer">
                  <div className="scanFailed" onClick={scanFailed}>
                    <FontAwesomeIcon icon={faBan} />
                  </div>
                  {runningStatus==0?
                    <div className='timerContainer'>
                      <div className="stopButton stopped" onClick={start}>
                        <FontAwesomeIcon icon={faPlay} />
                      </div>
                      <div className="seconds stopped">{Math.floor(milliseconds/100)}</div>
                    </div>
                    :
                    <div className='timerContainer'>
                      {runningStatus==1?
                        <div className="stopButton" onClick={stop}>
                          <FontAwesomeIcon icon={faStop} />
                        </div>
                        :
                        <div className="stopButton stopped" onClick={start}>
                          <FontAwesomeIcon icon={faPlay} />
                        </div>}
                      <div className={`seconds ${runningStatus==2?"stopped":""}`}>{Math.floor(milliseconds/100)}</div>
                    </div>
                  }
                  <div className="scanSuccess" onClick={scanSuccess}>
                    <FontAwesomeIcon icon={faCheck} />
                  </div>
                </div>
              </>
              

          </div>

          <div className="CSVContainer">
            {/* <input type="file" accept=".csv" onChange={e => readCSV(e)} /> */}
            {/* <FileUploader className="dragUploader" handleChange={readCSV} name="file" types={["CSV"]} /> */}
            {/* <div {...getRootProps()}>
              <input {...getInputProps()} />
              <p>Drag Here</p>
            </div> */}
            {/* <div ref={dropRef} >Drag Here</div> */}
            {/* <StyledDropzone props={} /> */}
            <div className='vaccinesContainer'>
              {vaccines.map((vaccine, arrIdx) => {
                let qrNum = arrIdx + 1;
                return(
                <div 
                  key={arrIdx}
                  className={`vaccine ${vaccineIndex==arrIdx?"clicked":vaccineStatus[(qrNum)]?"scanned":vaccineStatus[(qrNum)]==false?"failed":""}`}
                  onClick={()=>{
                    setVaccineIndex(arrIdx);
                    reset();
                  }}
                  style={vaccineIndex==arrIdx?
                          vaccineStatus[qrNum]?{background:`rgba(${FINAL_COLOR},1)`}:
                            vaccineStatus[qrNum]==false?
                              {background:`rgba(${FAILED_COLOR},1)`}
                              :{background:`rgba(${countdownRgbGradient(milliseconds, countdownSec*100, INITIAL_COLOR, FINAL_COLOR)},1)`}
                            :{}}>
                  <div className="vaccineInfo">
                    <div className="vaccineInfoNo">No.{qrNum}</div>
                    <div className="vaccineInfoDetails">
                      <div>Date: {vaccine['DateTime']}</div>
                      <div>ID: {vaccine['IDtag']}</div>
                    </div>
                  </div>
                </div>
                )
              })}
            </div>
          </div>
          </>
          }
          

        </div>
      </header>
    </div>
  );
}

export default App;
