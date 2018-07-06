/* compass.js - The JavaScript that drives the BuckoSoft compass.
 */
var compassVirginOnScreen;
var compassHelpSeen = 0;
var compassVirginTimeout = new Date();
var compassTextOnScreen = false;
var useIE = false;

var	compassFollowed = 0;
/* 0=not followed
 * 1=top
 * 2=up
 * 3=prev
 * 4=next
 * 5=skip1
 * 6=skip2
 * 7=skip3
 * 8=help
 */

var compassTextHelp = "Help: Go to the Sitemap";

function compassMouseOverTop() {
	if (useIE)
		document.getElementById("compassText").innerText = compassTextTop;
	else
		compassSetText(compassTextTop);
	compassMouseOver();
}
function compassMouseOverUp() {
	if (useIE)
		document.getElementById("compassText").innerText = compassTextUp;
	else
		compassSetText(compassTextUp);
	compassMouseOver();
}
function compassMouseOverPrev() {
	if (useIE)
		document.getElementById("compassText").innerText = compassTextPrev;
	else
		compassSetText(compassTextPrev);
	compassMouseOver();
}
function compassMouseOverNext() {
	if (useIE)
		document.getElementById("compassText").innerText = compassTextNext;
	else
		compassSetText(compassTextNext);
	compassMouseOver();
}
function compassMouseOverSkip1() {
	if (useIE)
		document.getElementById("compassText").innerText = compassTextSkip1;
	else
		compassSetText(compassTextSkip1);
	compassMouseOver();
}
function compassMouseOverSkip2() {
	if (useIE)
		document.getElementById("compassText").innerText = compassTextSkip2;
	else
		compassSetText(compassTextSkip2);
	compassMouseOver();
}
function compassMouseOverSkip3() {
	if (useIE)
		document.getElementById("compassText").innerText = compassTextSkip3;
	else
		compassSetText(compassTextSkip3);
	compassMouseOver();
}
function compassMouseOverHelp() {
	if (useIE)
		document.getElementById("compassText").innerText = compassTextHelp;
	else
		compassSetText(compassTextHelp);
	compassMouseOver();
}

/////////////
function compassMouseOver() {
	compassTextShow();
	compassVirginOnScreen = 0;
}

function compassMouseOut() {
	compassTextHide();
}

function compassMouseDownTop()	{ compassFollowed = 1; }
function compassMouseDownUp()	{ compassFollowed = 2; }
function compassMouseDownPrev()	{ compassFollowed = 3; }
function compassMouseDownNext() { compassFollowed = 4; }
function compassMouseDownSkip1(){ compassFollowed = 5; }
function compassMouseDownSkip2(){ compassFollowed = 6; }
function compassMouseDownSkip3(){ compassFollowed = 7; }
function compassMouseDownHelp()	{ compassFollowed = 8; }

function compassTextShow() {
	document.getElementById("compassText").style.visibility = "visible";
	if (document.getElementById("compassHelp") != null)
		document.getElementById("compassHelp").style.visibility = "visible";
	if (document.getElementById("compassBud") != null)
		document.getElementById("compassBud").style.visibility = "visible";
	compassTextOnScreen = true;
}
function compassTextHide() {
	document.getElementById("compassText").style.visibility = "hidden";
	if (document.getElementById("compassHelp") != null)
		document.getElementById("compassHelp").style.visibility = "hidden";
	if (document.getElementById("compassBud") != null)
		document.getElementById("compassBud").style.visibility = "hidden";
	compassTextOnScreen = false;
}


function compassInit()  {
//	compassFollowed=GetCookie("CompassFollowed");
	if (compassFollowed == null) {
		//alert(compassFollowed);
		compassMouseOver();
		compassVirginOnScreen = 1;
		compassVirginTimeout = new Date();
		compassVirginTimeout.setTime(compassVirginTimeout.getTime() + 4000);
		compassSetText("The compass will let you read this site like a book.<br />Just keep clicking next.");

		return;
	}
	//alert(compassFollowed);
	if (compassFollowed == 1) compassMouseOverTop();
	if (compassFollowed == 2) compassMouseOverUp();
	if (compassFollowed == 3) compassMouseOverPrev();
	if (compassFollowed == 4) compassMouseOverNext();
	if (compassFollowed == 5) compassMouseOverSkip1();
	if (compassFollowed == 6) compassMouseOverSkip2();
	if (compassFollowed == 7) compassMouseOverSkip3();
	if (compassFollowed == 8) {
		if (document.getElementById("compassHelp") != null)
			compassMouseOverHelp();
	}
	compassFollowed = 0;	// reset for this page
}

function compassUnload() {
//	SetCookie("CompassFollowed", compassFollowed, null, "/");
}

var compassMarqueeText = "";
var compassMarqueeIndex = -1;
var compassMarqueeDate;
var compassMarqueeMax;

function compassSetText(_t) {
	var _d = document.getElementById("compassText");
	var i, j;
	compassMarqueeText = _t;
	var t = "";
	for (i=0, j=0; i<_t.length; i++, j++) {
		if (_t.charAt(i) == '<') {
			t += '<';
			while (_t.charAt(++i) != '>') t += _t.charAt(i);
			continue;
		}
		t += '<span id="__c' + j + '">' +_t.charAt(i) + '</span>'; 
	}
	_d.innerHTML = t;
	compassMarqueeIndex = 0;
	compassMarqueeMax = j;
	compassMarqueeDate = new Date();
}

function compassTimer() {
	var now;
	var next = document.getElementById("compassNext");
	if (next == null)
		return;
	if (compassVirginOnScreen || compassMarqueeIndex > -1)
		now = new Date();
	if (compassVirginOnScreen) {
		compassTextShow();
		var blinkNext = (now.getTime() / 300) % 2;
		//document.getElementById("compassText").textContent = " " + ((now.getTime() / 400) % 2);
		if (blinkNext > 1)
			next.style.visibility = "visible";
		else
			next.style.visibility = "hidden";
//		if (now.getTime() > compassVirginTimout.getTime()) {
		if (compassVirginTimeout.getTime() < now.getTime()) {
			compassTextHide();
			compassVirginOnScreen = 0;
			next.style.visibility = "visible";
		}
	}
	if (compassMarqueeIndex > -1 && compassTextOnScreen) {
		var tm = (now.getTime() - compassMarqueeDate.getTime()) / 100;
		if (tm > compassMarqueeIndex) {
			var _d = document.getElementById("__c" + compassMarqueeIndex);
			if (_d == null) {
				//compassMarqueeIndex = 0;
				return;
			}
			_d.style.fontSize = "16";
			if (++compassMarqueeIndex >= compassMarqueeMax) {
				compassMarqueeIndex = -1;
			} else {
				_d = document.getElementById("__c" + compassMarqueeIndex);
				if (_d != null)
					_d.style.fontSize = "18";
				_d = document.getElementById("__c" + (compassMarqueeIndex+1));
				if (_d != null)
					_d.style.fontSize = "20";
				_d = document.getElementById("__c" + (compassMarqueeIndex+2));
				if (_d != null)
					_d.style.fontSize = "22";
			}
		}
	}
}

