
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

function sipcb_alert() {
	alert("Sent from Elsipo framework");
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
	//alert("new text message from " + from);
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
	var uri = document.getElementById('i_address').value;
	if(sip.doCall(uri)<0) {
		alert("Failed calling to: " + uri);
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
