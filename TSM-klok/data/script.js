var gateway = `ws://${window.location.hostname}/ws`;
let ws = new WebSocket(gateway);

ws.addEventListener('message', function (event) {
    let data = JSON.parse(event.data);
    if ("info" in data) {
        alert(data["info"]);
    } else {
        document.getElementById("SSID").value = (data["ssid"] != null) ? data["ssid"] : "";
        alert(`ssid=${data["ssid"]}`);
        document.getElementById("PSK").value = (data["psk"] != null) ? data["psk"] : "";
    }
});

function dispDt() {
    document.getElementById("dt").style.visibility = document.getElementById("auto").checked ? "visible" : "hidden";
} 

function dispInt() {
    document.getElementById("intensiteit").style.visibility = document.getElementById("autoInt").checked ? "visible" : "hidden";
}

function getColor() {
    let colors = document.getElementsByName("color");
    var selected;
    for (var i = 0; i < colors.length; i++) {
        if (colors[i].checked) {
            selected = colors[i].id;
        }
    }
    if (selected === "cA") {
        return "auto";
    } else if (selected === "cW") {
        return "white";
    } else {
        let labels = document.getElementsByTagName('label');
        for (var i = 0; i < labels.length; i++) {
            if (labels[i].htmlFor == selected) return labels[i].style.color;
        }
    }
}

function TestLed() {
    let color = getColor();
    let intensity = document.getElementById("autoInt").checked ? "auto" : document.getElementById("intensiteit").value;
    ws.send(`{"testLed":{"color":"${color}", "intensity":"${intensity}"}}`);
}

function TestAudio() {
    let volume = document.getElementById("volume").value;
    ws.send(`{"testVolume":{"volume":${volume}}}`);
}

function verzend(forced) {
    const ssid = document.getElementById("SSID").value;
    const psk = document.getElementById("PSK").value;
    if ((ssid === "" || psk === "") && (!forced)) {
        alert("Wachtwoord en SSID zijn verplicht");
        return;
    }
    let color = getColor();
    let intensiteit = document.getElementById("autoInt").checked ? document.getElementById("intensiteit").value : "auto";
    let dateTime = document.getElementById("auto").checked ? document.getElementById("dt").value : "auto";
    let volume = document.getElementById("volume").value;
    let data = `{"config":{"SSID":"${ssid}", "PSK":"${psk}", "color":"${color}", "intensity":"${intensiteit}", "datetime":"${dateTime}", "volume":${volume}}}`;
    ws.send(data);
} 

function deleteConfig() {
    document.getElementById("SSID").value = "";
    document.getElementById("PSK").value = "";
    document.getElementById("cA").checked = true;
    document.getElementById("autoInt").checked = false;
    document.getElementById("intensiteit").value = 100;
    dispInt();
    document.getElementById("volume").value = 100;
    document.getElementById("auto").checked = false;
    dispDt();
    verzend(true);
}


window.onload = () => {
    if (screen.width >= screen.height) {
        document.body.style.marginLeft = "33%";
        document.body.style.marginRight = "33%";
    }
}