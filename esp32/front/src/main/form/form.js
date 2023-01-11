// Css and Scss added to entry point
import * as bootstrap from 'bootstrap';
window.Toast = bootstrap.Toast;

import "./form.css";
import "./bootstrap.scss";

// Js files added to entry poitn


// branches of entry point
//import "../download/download";



//DESCOMENTAR CODIGO PARA TENER UNA VENTANA PROPIA PARA DESCARGA, EN LUGAR DE SOLO UNA ALARMA.
// EN INDEX.HTML TAMBIEN DESCOMENTAR LA SECCION "DESCARGA VIEW" Y COMENTAR "MODAL FADE"

// INIT BOOTSTRAP TOAST
var toastElList = [].slice.call(document.querySelectorAll('.toast'))
toastElList.map(function (toastEl) {
    return new Toast(toastEl, Toast.Default);    
})

function resetToast(header, body, status) {
    // get variables
    var liveToastTag = document.getElementById("liveToast");
    var liveToast = Toast.getInstance(liveToastTag);

    // hide toast
    //liveToast.hide(); // is async

    // update toast content
    liveToastTag.getElementsByTagName("strong")[0].innerText = header;
    liveToastTag.getElementsByClassName("toast-body")[0].innerText = body;

    if (status === "icon-success") {
        liveToastTag.getElementsByTagName("use")[0].setAttribute("xlink:href", "#check-circle-fill");
    } else if (status === "icon-failed") {
        liveToastTag.getElementsByTagName("use")[0].setAttribute("xlink:href", "#exclamation-triangle-fill");
    } else {
        liveToastTag.getElementsByTagName("use")[0].setAttribute("xlink:href", "#info-fill");
    }

    // show toast
    liveToast.show();
}

// ACTION FOR SEND FORM
document.getElementById("SendForm").onclick = async function () {

    var SmartFarmID = document.getElementById("SmartFarmID").value;
    var EndPoint = document.getElementById("EndPoint").value;
    var LoraAppEui = document.getElementById("LoraAppEui").value;
    var LoraDevEui = document.getElementById("LoraDevEui").value;
    var LoraAppKey = document.getElementById("LoraAppKey").value;
    var WifiPassword = document.getElementById("WifiPassword").value;

    let data = {
        "SmartFarmID": SmartFarmID,
        "EndPoint": EndPoint,
        "LoraAppEui": LoraAppEui,
        "LoraDevEui": LoraDevEui,
        "LoraAppKey": LoraAppKey,
        "WifiPassword": WifiPassword,
    };
    console.log(data);

    var response = await fetch("https://91a0e5d7-7a05-4c17-9424-e884dbb20f96.mock.pstmn.io/form", {
        method: "POST",
        body: JSON.stringify(data),
        headers: {
            'Content-Type': 'application/json'
        }
    });

    let body = await response.text();
    if (response.status === 200) {
        resetToast("Send Form", body, "icon-success");
    } else {
        resetToast("Send Form", body, "icon-failed");
    }
};

// ACTION FOR TEST UPLINK
document.getElementById("TestUplink").onclick = async function () {

    var response = await fetch("https://90697ef9-da35-4722-a815-3b692119f221.mock.pstmn.io/testuplink", {
        method: "GET",      
    });

    let body = await response.text();
    if (response.status === 200) {
        resetToast("Test Uplink", body, "icon-success");
    } else {
        resetToast("Test Uplink", body, "icon-failed");
    }
};