(function ($, Drupal, window, document, undefined) {
  
  $(document).ready(function (){
    initialize();  
  });
    
  var map,allowedBounds;
  
    
  function initialize() {
    
    var redpin, ghostpin, x, infobox;
    var zoomControl=true;
    var scrollwheel=true;
    
    
    if(Drupal.settings.viewmode=='mini'){
      zoomControl=false;
      scrollwheel=false;
    }
    
    //core map options
    var mapOptions = { 
      center: new google.maps.LatLng(Drupal.settings.lat, Drupal.settings.lng),
      zoom: Drupal.settings.zoom,
      minZoom: 10,
      maxZoom: 20,
      mapTypeId: google.maps.MapTypeId.ROADMAP,  
      mapTypeControl: true,
      mapTypeControlOptions: {
        mapTypeIds:[google.maps.MapTypeId.ROADMAP,google.maps.MapTypeId.SATELLITE]
      },
      panControl:false,
      zoomControl: zoomControl,
      zoomControlOptions: {
        style: google.maps.ZoomControlStyle.SMALL
      },
      streetViewControl:false,
      draggable:true,
      scrollwheel:scrollwheel,
    }
    
    // make the map
    map = new google.maps.Map(document.getElementById("map-canvas"),mapOptions);
        
    // make it purty    
    var styles = getMapStyle();
    map.setOptions({styles: styles});
    
    //default to non-45 degree view
    map.setTilt(0);   
            
    //define the custom pin icons
    var redpinicon = new google.maps.MarkerImage('/sites/all/themes/historicalsurvey_theme/img/pin-redball.png',
        new google.maps.Size(10,17),
        new google.maps.Point(0,0),
        new google.maps.Point(5,17)
    );
    var ghostpinicon = new google.maps.MarkerImage('/sites/all/themes/historicalsurvey_theme/img/pin-ghostball.png',
        new google.maps.Size(10,17),
        new google.maps.Point(0,0),
        new google.maps.Point(5,17)
    );
    
    //initalize auto-boundary set
    if(Drupal.settings.redpins.length>0){
      var bounds = new google.maps.LatLngBounds();
    }
    
    //pin the ghostpins, if any
    for(x=0;x<Drupal.settings.ghostpins.length;x++){
      
      var point = new google.maps.LatLng(Drupal.settings.ghostpins[x]['lat'],Drupal.settings.ghostpins[x]['lng']);

      ghostpin=new google.maps.Marker({
        position: point,
        map:map,
        icon:ghostpinicon
      });
      
      google.maps.event.addListener(ghostpin, 'click', (function(ghostpin,x) {
        return function() {
          var infoboxOptions = {
             content: Drupal.settings.ghostpins[x]['content'],
             disableAutoPan: false,
             pixelOffset: new google.maps.Size(-100,0),		
             zIndex: null,
             boxStyle: { 
              background: "url('/sites/all/themes/historicalsurvey_theme/img/tipbox.png') no-repeat",
              width: "215px"
             },
             closeBoxMargin: "12px 12px -10px 2px",
             closeBoxURL: "/sites/all/themes/historicalsurvey_theme/img/close.png",
             infoBoxClearance: new google.maps.Size(1, 1),
             isHidden: false,
             pane: "floatPane",
             enableEventPropagation: false
          };
          if(infobox){
            infobox.close();
          }
          infobox = new InfoBox(infoboxOptions);		
          infobox.open(map, ghostpin);          
        }
      })(ghostpin,x));
    }
    
    //pin the redpins
    for(x=0;x<Drupal.settings.redpins.length;x++){

      var point = new google.maps.LatLng(Drupal.settings.redpins[x]['lat'],Drupal.settings.redpins[x]['lng']);

      //add point to auto-boundaries
      bounds.extend(point);      
      
      var markeroptions = {
        position: point,
        map:map,
        icon:redpinicon
      }
      if(Drupal.settings.viewmode=='create'){
        markeroptions['draggable'] = true;
      }
      
      redpin=new google.maps.Marker(markeroptions);
      
      google.maps.event.addListener(redpin, 'click', (function(redpin,x) {
        return function() {    
          if(Drupal.settings.redpins[x]['content']!=''){
            var infoboxOptions = {
               content: Drupal.settings.redpins[x]['content'],
               disableAutoPan: false,
               pixelOffset: new google.maps.Size(-100,0),		
               zIndex: null,
               boxStyle: { 
                background: "url('/sites/all/themes/historicalsurvey_theme/img/tipbox.png') no-repeat",
                width: "215px"
               },
               closeBoxMargin: "12px 12px 2px 2px",
               closeBoxURL: "/sites/all/themes/historicalsurvey_theme/img/close.png",
               infoBoxClearance: new google.maps.Size(1, 1),
               isHidden: false,
               pane: "floatPane",
               enableEventPropagation: false
            };
            if(infobox){
              infobox.close();
            }
            infobox = new InfoBox(infoboxOptions);		
            infobox.open(map, redpin);          
          }
        }
      })(redpin,x));
    }
    
    
    // close the open infobox when clicking on map
    google.maps.event.addListener(map, 'click', (function() {
      if(infobox){
        infobox.close();
      }
    }));
    
    
    //if this is the map in the create new place form, allow users to select points
    if(Drupal.settings.viewmode=='create'){      
      //if geocoder found a point, record it
      setLatLng(redpin.getPosition());

      //click to re position point
      google.maps.event.addListener(map, 'click', function(event) {
        redpin.setPosition(event.latLng);             
      });  
      //record new points
      google.maps.event.addListener(redpin, 'position_changed', function(event) {       
        setLatLng(redpin.getPosition());      
      });    
    }
    
      
    //center the map
    if(Drupal.settings.viewmode=='main') {
      map.setZoom(13);
    }
    
    if(Drupal.settings.viewmode=='search'){
      map.fitBounds(bounds);      
    }
    
    if(Drupal.settings.viewmode=='mini'){      
      map.setCenter(bounds.getCenter());
      map.setZoom(17);
    }
      
    if(Drupal.settings.viewmode=='create'){      
      map.setCenter(bounds.getCenter());
      map.setZoom(18);
    }
    
    //dont let users pan outside the allowedbounds
    if(Drupal.settings.viewmode=='mini'){
      var miniCenter = map.getCenter();
      
      var minY = miniCenter.lat()-0.0015;
      var maxY = miniCenter.lat()+0.0015;
      var minX = miniCenter.lng()-0.0015;
      var maxX = miniCenter.lng()+0.0015;
      
      allowedBounds = new google.maps.LatLngBounds(new google.maps.LatLng(minY, minX),new google.maps.LatLng(maxY, maxX));    
      google.maps.event.addListener(map,'center_changed',function() {checkBounds(); });
    }
    else{     
      //set allowedbounds to rectangle reasonably around the city
      allowedBounds = new google.maps.LatLngBounds(new google.maps.LatLng(29.913, -98.513),new google.maps.LatLng(30.659, -96.979));    
      google.maps.event.addListener(map,'center_changed',function() {checkBounds(); });
    }
  }
  
  /*checks to make sure users stay within allowedbounds*/
  function checkBounds() {
    if(!allowedBounds.contains(map.getCenter())) {
      var C = map.getCenter();
      var X = C.lng();
      var Y = C.lat();

      var AmaxX = allowedBounds.getNorthEast().lng();
      var AmaxY = allowedBounds.getNorthEast().lat();
      var AminX = allowedBounds.getSouthWest().lng();
      var AminY = allowedBounds.getSouthWest().lat();

      if (X < AminX) {X = AminX;}
      if (X > AmaxX) {X = AmaxX;}
      if (Y < AminY) {Y = AminY;}
      if (Y > AmaxY) {Y = AmaxY;}

      map.setCenter(new google.maps.LatLng(Y,X));
    }
  }  
  
  
  /*set the lat lng into the html form elements for delivery*/
  function setLatLng(latLng){
    $("input#edit-selectedLat").attr("value",latLng.lat());
    $("input#edit-selectedLng").attr("value",latLng.lng());
  }
  
  
  //created with help from http://gmaps-samples-v3.googlecode.com/svn/trunk/styledmaps/wizard/index.html
  function getMapStyle(){
    return [
      {
        "featureType": "administrative.land_parcel",
        "stylers": [
          { "weight": 0.7 },
          { "color": "#808080" },
          { "visibility": "on" },
          { "lightness": 66 }
        ]
      },{
        "featureType": "administrative.neighborhood",
        "stylers": [
          { "lightness": -89 },
          { "visibility": "off" }
        ]
      },{
        "featureType": "administrative.locality",
        "stylers": [
          { "visibility": "off" }
        ]
      },{
        "featureType": "administrative.province",
        "stylers": [
          { "visibility": "off" }
        ]
      },{
        "featureType": "administrative.country",
        "stylers": [
          { "visibility": "off" }
        ]
      },{
        "featureType": "landscape.man_made",
        "elementType": "geometry.stroke",
        "stylers": [
          { "color": "#000000" },
          { "visibility": "on" }
        ]
      },{
        "featureType": "landscape.natural.landcover",
        "stylers": [
          { "visibility": "off" }
        ]
      },{
      },{
        "featureType": "landscape.natural.terrain",
        "stylers": [
          { "lightness": -91 },
          { "visibility": "off" }
        ]
      },{
        "featureType": "poi.attraction",
        "stylers": [
          { "visibility": "off" }
        ]
      },{
        "featureType": "poi.business",
        "stylers": [
          { "visibility": "off" }
        ]
      },{
        "featureType": "poi.government",
        "elementType": "geometry",
        "stylers": [
          { "visibility": "on" },
          { "saturation": -42 }
        ]
      },{
        "featureType": "poi.medical",
        "stylers": [
          { "visibility": "off" }
        ]
      },{
        "featureType": "poi.park",
        "stylers": [
          { "visibility": "on" },
          { "lightness": 44 }
        ]
      },{
        "featureType": "poi.place_of_worship",
        "stylers": [
          { "visibility": "off" }
        ]
      },{
        "featureType": "poi.school",
        "stylers": [
          { "visibility": "simplified" },
          { "lightness": 100 }
        ]
      },{
        "featureType": "poi.sports_complex",
        "stylers": [
          { "color": "#7f8080" },
          { "visibility": "off" }
        ]
      },{
        "featureType": "road.highway",
        "stylers": [
          { "visibility": "simplified" }
        ]
      },{
        "featureType": "road.highway",
        "elementType": "geometry.fill",
        "stylers": [
          { "color": "#808080" },
          { "lightness": 74 }
        ]
      },{
        "featureType": "transit.line",
        "stylers": [
          { "lightness": -65 }
        ]
      },{
        "featureType": "transit.station",
        "stylers": [
          { "visibility": "off" }
        ]
      },{
        "featureType": "water",
        "stylers": [
          { "lightness": -52 }
        ]
      },{
      },{
        "featureType": "landscape.man_made",
        "elementType": "geometry.fill",
        "stylers": [
          { "visibility": "on" },
          { "lightness": 100 }
        ]
      },{
        "featureType": "landscape.man_made",
        "elementType": "labels",
        "stylers": [
          { "visibility": "off" }
        ]
      },{
        "featureType": "water",
        "elementType": "labels",
        "stylers": [
          { "visibility": "off" }
        ]
      },{
        "featureType": "road.arterial",
        "elementType": "geometry.fill",
        "stylers": [
          { "lightness": 78 },
          { "color": "#ffffff" }
        ]
      },{
        "featureType": "road.arterial",
        "elementType": "geometry.stroke",
        "stylers": [
          { "visibility": "on" },
          { "color": "#808080" },
          { "lightness": 65 }
        ]
      },{
        "featureType": "road.arterial",
        "elementType": "labels.icon",
        "stylers": [
          { "visibility": "off" }
        ]
      },{
        "featureType": "road.arterial",
        "elementType": "labels.text.stroke",
        "stylers": [
          { "visibility": "off" }
        ]
      },{
        "featureType": "road.arterial",
        "elementType": "labels.text.fill",
        "stylers": [
          { "color": "#808080" }
        ]
      },{
        "featureType": "road.local",
        "elementType": "labels.text.fill",
        "stylers": [
          { "color": "#808080" }
        ]
      },{
        "featureType": "road.local",
        "elementType": "geometry.fill",
        "stylers": [
          { "color": "#ffffff" }
        ]
      },{
        "featureType": "poi.park",
        "elementType": "labels",
        "stylers": [
          { "visibility": "off" }
        ]
      },{
        "featureType": "poi.government",
        "elementType": "labels",
        "stylers": [
          { "visibility": "off" }
        ]
      },{
        "featureType": "poi.sports_complex",
        "elementType": "labels",
        "stylers": [
          { "visibility": "off" }
        ]
      },{
        "featureType": "poi",
        "elementType": "labels",
        "stylers": [
          { "visibility": "off" }
        ]
      }
    ];    
  }

})(jQuery, Drupal, this, this.document);
