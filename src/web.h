#include <Arduino.h>
const char MAIN_page1[] PROGMEM = R"=====(<!DOCTYPE html>
<html>
    <head>
        <style>
            div{
                margin: 5px;
                text-align: center;
            }
            input[type="range"]{
                writing-mode: bt-lr; /* IE */
                -webkit-appearance: slider-vertical; /* WebKit */
                width: 25px;
                background:red;
            }
            body{
                background-color:black;
            }
            p{
                color:greenyellow;
                font-family:monospace;
                font-size:smaller;
            }
            legend{
                color:hotpink
            }
        </style>
    </head>
    <body>
        <p style="color:snow" id="time"></p>
        <fieldset id="am">
            <legend>AM</legend>
        </fieldset>
        <fieldset id="pm">
            <legend>PM</legend>
        </fieldset>
        <fieldset id="cool">
            <legend>fan settings</legend>
        </fieldset>
        <script>
            function slided(target){
                var pp = target.parentElement.getElementsByTagName('p')[0]
                pp.innerText = target.value;
                pp.style.color=`rgb(255, ${140+(+target.value)}, ${target.value*2})`
            }
            function update(query){
                var xhr = new XMLHttpRequest();
                xhr.open("GET",location.origin+query);
                xhr.send();
                xhr.onload = ()=>{
                    location.reload();
                }
            }
            function update_light ({id , value}){
                console.log("light",id,value);
                update(`/setled?time=${id}&brightness=${value}`)
            }
            function update_setting({id , value}){
                console.log("setting",id,value);
                update(`/setting?name=${id}&value=${value}`)
            }
)=====";


const char MAIN_page2[] PROGMEM = R"=====(
            document.getElementById("time").innerText = `TIME: ${curTime}
            AIR : ${airTemp} *C , RH: ${airRH} %
            LED : ${ledTemp} *C`
            for(var i=0;i<24;i++){
                var ranger = document.createElement("div");
                var hr = (i>=12)?i-12:i;
                ranger.innerHTML = `<div style="float: left;">
                    <p style="color:darkorange">50</p>
                    <input id="${i}" oninput="slided(this)" onchange="update_light(this)" type="range" min="0" max="100" value="${hrs[i]}"/>
                    <p >${hr}-${hr+1}</p>
                </div>`
                if(i < 12){
                    document.getElementById('am').appendChild(ranger);
                }
                else{
                    document.getElementById('pm').appendChild(ranger);
                }
                
            }
            for(var id in setting){
                var ranger = document.createElement("div");
                var {desc,min,max,step,val} = setting[id];
                ranger.innerHTML = `<div style="float: left;">
                    <p style="color:darkorange">50</p>
                    <input style="height:200px;" id="${id}" oninput="slided(this)" onchange="update_setting(this)" type="range" step=${step} min=${min} max=${max} value="${val}"/>
                    <p >${desc}</p>
                </div>`;
                document.getElementById('cool').append(ranger);
            }
            for(var s of document.querySelectorAll('input[type="range"]')){
                s.oninput();
            }

        </script>
    </body>
</html>)=====";

#define hrAday 24
#include <routines.h>

String getFullPage(uint8_t hours[hrAday],float RH,float temp_air,float temp_led,float fanTemp_led,float fanTemp_air,uint8_t collect_interval_minute){
    String cloned1 = FPSTR(MAIN_page1);
    String cloned2 = FPSTR(MAIN_page2);
    String hrs = "[";
    for(int i=0;i<hrAday;i++){
        hrs += (String)hours[i]+",";
    }
    hrs += "]";
    String vars[6][2] = {
    {"hrs",hrs},
    {"curTime",'\"'+timeStr()+'\"'},
    {"airTemp",String(temp_air)},
    {"airRH",String(RH)},
    {"ledTemp",String(temp_led)},
    {"setting","{led:{desc:\"led temp(C)\",min:30,max:80,step:0.1,val:"+String(fanTemp_led)+"},air:{desc:\"air temp(C)\",min:20,max:80,step:0.1,val:"+fanTemp_air+"},collect_interval:{desc:\"collect_interval(min)\",min:1,max:60,step:1,val:"+collect_interval_minute+"}}"}
}   ;

    String variables = "";
    for(int j=0;j<6;j++){
        variables += "var "+vars[j][0]+" = "+vars[j][1]+";\n";
    }
    return cloned1+variables+cloned2;
}