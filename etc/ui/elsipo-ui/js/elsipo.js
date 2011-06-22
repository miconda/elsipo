
// alert( 'Plugin loaded: ' + sip.name() + ' v' + sip.version());

function sipcb_register_ok() {
	if(sip.getSIPRegistrationStatus()==1) {
		document.getElementById('p_register').innerHTML="Registration succeeded - you are online";
		document.getElementById('b_register').value="Click to unregister";
		document.getElementById('b_register').disabled=false;
	} else if(sip.getSIPRegistrationStatus()==3) {
		document.getElementById('p_register').innerHTML="Unregistration succeeded - you are offline";
		document.getElementById('b_register').value="Click to register";
		document.getElementById('b_register').disabled=false;
	}
	//alert("user registered");
}

function sipcb_register_failed() {
	document.getElementById('p_register').innerHTML="Registration failed - you are offline";
	document.getElementById('b_register').value="Click to register";
	document.getElementById('b_register').disabled=false;
	//alert("user unregistered");
}

function sipcb_alert(text) {
	alert("By Elsipo: " + text);
}

function sipjs_parse_sip_uri(strURI) {
    var uriParts = {
		user: null,
		domain: null,
		ext: null
    };
     
    strURI.replace(new RegExp( "^<?(sip[s]?:)([^@]+)@(.+)>?$" , "i" ),
		function( $0, $1, $2, $3 ){
			uriParts.user = $2;
			uriParts.domain = $3;
		}
    );
     
    return( uriParts );
}

function sipjs_get_user_domid(strID) {
	var domID = null;
	if(strID==null)
		return null;
    strID.replace(new RegExp( "^(id-)([^@]+)$" , "i" ),
		function( $0, $1, $2 ){
			domID = $2;
		}
    );
    return( domID );
}

function sipcb_message(from, ctype, body) {
	//alert("new text message from [" + from + "] [" + body + "]");
	uri = sipjs_parse_sip_uri(from);

	docid = sipjs_get_user_domid(uri.user);
	if(docid != null) {
		//alert("updating in dom id " + docid);
		document.getElementById(docid).innerHTML = body;
	} else {
		var text = document.getElementById('i_imdata').value;
		document.getElementById('i_imdata').value = text + "++ " + from + ": " + body + "\n";
	}
}

function sipcb_message_failed(uri, code, reason) {
	var text = document.getElementById('i_imdata').value;
	document.getElementById('i_imdata').value = text + "** failed to send IM to "
		+ uri + " (" + code + "/" + reason + ")\n";
}

function sipjs_do_registration() {
	if(sip.doSIPRegistration()<0)
	{
		document.getElementById('b_register').disabled=false;
		document.getElementById('b_register').value="Click to register";
		document.getElementById('p_register').innerHTML="Registration failed...";
	} else {
		document.getElementById('b_register').disabled=true;
		document.getElementById('b_register').value="Wait...";
		document.getElementById('p_register').innerHTML="Registration in progress...";
	}
}

function sipjs_do_call() {
	var action = document.getElementById('b_action').value;
	if(action=="Dial") {
		var uri = document.getElementById('i_address').value;
		if(sip.doCall(uri)<0) {
			alert("Failed calling to: " + uri);
			return;
		}

		document.getElementById('b_action').disabled=true;
		document.getElementById('b_action').value="Calling...";
		document.getElementById('b_reset').disabled=false;
		document.getElementById('b_reset').value="Hang Up";
		return;
	}
	
	if(action=="Answer") {
		if(sip.doCallAnswer()<0) {
			document.getElementById('b_action').disabled=false;
			document.getElementById('b_action').value="Dial";
			document.getElementById('b_reset').disabled=true;
			document.getElementById('b_reset').value="...";
			document.getElementById('i_address').value = "Type the address to dial...";

			alert("Failed to answer the call");
			return;
		}
		document.getElementById('b_action').disabled=true;
		document.getElementById('b_action').value="Answered...";
		document.getElementById('b_reset').disabled=false;
		document.getElementById('b_reset').value="Hang Up";
	}
}

function sipjs_do_call_reject() {
	var uri = document.getElementById('i_address').value;
	document.getElementById('b_action').disabled=false;
	document.getElementById('b_action').value="Dial";
	document.getElementById('b_reset').disabled=true;
	document.getElementById('b_reset').value="...";
	document.getElementById('i_address').value = "Type the address to dial...";
	if(sip.doCallReject()<0) {
		alert("Failed ending the call");
	}
}

function sipjs_send_message() {
	var uri = document.getElementById('i_imaddress').value;
	var msg = document.getElementById('i_imtext').value;
	// alert("Sending IM to: " + uri + " [" + msg + "]");
	var text = document.getElementById('i_imdata').value;
	document.getElementById('i_imdata').value = text + "-- " + uri + ": " + msg + "\n";
	document.getElementById('i_imtext').value = "";
	if(sip.sendIM(uri, msg)<0)
	{
		alert("Error while sending the IM to " + uri);
	}
}

function sipjs_send_xquery(uri, text) {
	sip.sendIM(uri, text);
}


function sipcb_call_in(uri) {
	alert("Incoming call from: " + uri);
	document.getElementById('b_action').disabled=false;
	document.getElementById('b_action').value="Answer";
	document.getElementById('b_reset').disabled=false;
	document.getElementById('b_reset').value="Hang Up";
	document.getElementById('i_address').value = "Call from: " + uri;
}

function sipcb_call_ended() {
	document.getElementById('b_action').disabled=false;
	document.getElementById('b_action').value="Dial";
	document.getElementById('b_reset').disabled=true;
	document.getElementById('b_reset').value="...";
	document.getElementById('i_address').value = "Type the address to dial...";
}
