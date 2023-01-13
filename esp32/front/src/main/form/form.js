// Css and Scss added to entry point
import * as bootstrap from 'bootstrap';
window.Toast = bootstrap.Toast;

import "./form.css";
import "./bootstrap.scss";


let base_url = "http://192.168.4.1";
let wifiscan_path = "/api/wifiscan";
let testuplink_path = "/api/testuplink";
let save_path = "/api/save";

// INIT BOOTSTRAP TOAST
var toastElList = [].slice.call(document.querySelectorAll('.toast'))
toastElList.map(function (toastEl) {
    return new Toast(toastEl, { delay: 3000 });
})

// RESET SELECT CONTENT
function resetSelect(array) {
    let SelectWifiSsid = document.getElementById('SelectWifiSsid');

    // remove all options
    let old_options = SelectWifiSsid.getElementsByTagName("option");
    Array.from(old_options).forEach(option => {
        SelectWifiSsid.removeChild(option);
    });

    // add default option
    var default_opt = document.createElement('option');
    default_opt.text = "Select SSID";
    default_opt.selected = true;
    default_opt.disabled = true;
    SelectWifiSsid.appendChild(default_opt);

    // add new ssid
    array.forEach(ssid => {
        var opt = document.createElement('option');
        opt.text = ssid;
        SelectWifiSsid.appendChild(opt);
    });

}

// RESET CONTENT TOAST
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
document.getElementById("Save").onclick = async function () {

    var SensorID = document.getElementById("SensorID").value;
    var EndPoint = document.getElementById("EndPoint").value;
    var LoraAppEui = document.getElementById("LoraAppEui").value;
    var LoraDevEui = document.getElementById("LoraDevEui").value;
    var LoraAppKey = document.getElementById("LoraAppKey").value;
    var WifiSsid = document.getElementById("SelectWifiSsid").selectedOptions[0].text;
    var WifiPassword = document.getElementById("WifiPassword").value;

    let data = {
        "SensorID": SensorID,
        "EndPoint": EndPoint,
        "LoraAppEui": LoraAppEui,
        "LoraDevEui": LoraDevEui,
        "LoraAppKey": LoraAppKey,
        "WifiSsid": WifiSsid,
        "WifiPassword": WifiPassword,
    };
    console.log(data);

    var response = await fetch(base_url + save_path, {
        method: "POST",
        body: JSON.stringify(data),
        headers: {
            'Content-Type': 'application/json'
        }
    });

    let body = await response.json();
    if (response.status === 200) {
        resetToast("Save ", body["info"], "icon-success");
    } else {
        resetToast("Save ", body["info"], "icon-failed");
    }
};

// ACTION FOR TEST UPLINK
document.getElementById("TestUplink").onclick = async function () {

    var response = await fetch(base_url + testuplink_path, {
        method: "GET",
    });

    let body = await response.json();
    if (response.status === 200) {
        resetToast("Test Uplink ", body["info"], "icon-success");
    } else {
        resetToast("Test Uplink ", body["info"], "icon-failed");
    }
};

// ACTION FOR SCAN WIFI
document.getElementById("ScanWifi").onclick = async function () {

    var response = await fetch(base_url + wifiscan_path, {
        method: "GET",
    });

    let body = await response.json();
    if (response.status === 200) {
        resetToast("Scan Wifi ", body["info"], "icon-success");
        resetSelect(body["payload"]);
    } else {
        resetToast("Scan Wifi ", body["info"], "icon-failed");
    }
};

