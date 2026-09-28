// shims.js - Replaces api-helper.js, api-helper-auth.js, api-helper-usb.js,
// api-helper-cloud.js, api-helper-extras.js, particle.min.js
// Provides minimal apiHelper surface for WiFi-only mode.
(function() {
  'use strict';
  window.analytics = { track: function(){}, page: function(){}, identify: function(){} };
  window.Cookies = { get: function(){ return null; }, set: function(){}, remove: function(){} };
  var apiHelper = window.apiHelper || {};
  window.apiHelper = apiHelper;

  // ---- Version utilities ----
  apiHelper.parseVersionStr = function(v) {
    var r = {};
    while (v && (v.charAt(0) < '0' || v.charAt(0) > '9')) v = v.substr(1);
    var p = v.split(/[-.]/);
    if (p.length < 2) return r;
    r.major = parseInt(p[0]);
    r.minor = parseInt(p[1]) || 0;
    r.patch = parseInt(p[2]) || 0;
    if (p.length > 4) r.pre = parseInt(p[4]);
    if (p.length > 3) {
      if (p[3] === 'rc') { r.rc = r.pre; r.preAdj = r.pre * 400; }
      else if (p[3] === 'alpha') { r.alpha = r.pre; r.preAdj = r.pre * 200; }
      else if (p[3] === 'beta') { r.beta = r.pre; r.preAdj = r.pre * 300; }
      else if (p[3] === 'test') { r.test = r.pre; r.preAdj = r.pre * 100; }
    }
    return r;
  };

  apiHelper.versionSort = function(a, b) {
    var aa = apiHelper.parseVersionStr(a), bb = apiHelper.parseVersionStr(b), c;
    c = bb.major - aa.major; if (c) return c;
    c = bb.minor - aa.minor; if (c) return c;
    c = bb.patch - aa.patch; if (c) return c;
    if (!aa.pre && !bb.pre) return 0;
    return aa.pre ? (bb.pre ? bb.preAdj - aa.preAdj : 1) : -1;
  };

  // ---- Embedded device platform data (replaces fetch('/assets/files/deviceRestore.json')) ----
  var platforms = [
    {id:12,name:'argon',mcu:'nRF52840',wifi:true,cellular:false,gen:3},
    {id:26,name:'tracker',mcu:'nRF52840',wifi:false,cellular:true,gen:3},
    {id:28,name:'trackerM',mcu:'nRF52840',wifi:false,cellular:true,gen:3},
    {id:32,name:'p2',mcu:'RTL8721DM',wifi:true,cellular:false,gen:3,wifiSelectAntenna:true},
    {id:33,name:'msom',mcu:'RTL8721DM',wifi:true,cellular:true,gen:3,wifiSelectAntenna:true},
    {id:34,name:'photon2',mcu:'RTL8721DM',wifi:true,cellular:false,gen:3,wifiSelectAntenna:true},
    {id:35,name:'monitorM',mcu:'nRF52840',wifi:false,cellular:true,gen:3},
    {id:13,name:'boron',mcu:'nRF52840',wifi:false,cellular:true,gen:3},
    {id:23,name:'bsom',mcu:'nRF52840',wifi:false,cellular:true,gen:3},
    {id:25,name:'b5som',mcu:'nRF52840',wifi:false,cellular:true,gen:3},
    {id:14,name:'bseries',mcu:'STM32F205',wifi:false,cellular:true,gen:2},
    {id:10,name:'electron',mcu:'STM32F205',wifi:false,cellular:true,gen:2},
    {id:37,name:'electron2',mcu:'nRF52840',wifi:false,cellular:true,gen:3},
    {id:8,name:'photon',mcu:'STM32F205',wifi:true,cellular:false,gen:2},
    {id:0,name:'core',mcu:'STM32F103',wifi:false,cellular:false,gen:1}
  ];
  var legacyVersions = ['3.0.0','3.1.0','3.2.0','3.3.0','3.3.1','4.0.0','4.0.1','4.0.2','4.1.0',
    '5.0.0','5.0.1','5.1.0','5.2.0','5.3.0','5.3.1','5.3.2','5.4.0','5.5.0','5.6.0','5.7.0',
    '5.8.0','5.9.0','6.0.0','6.0.1','6.1.0','6.1.1','6.2.0'];
  var verByPf = {
    argon: legacyVersions,
    p2: ['5.0.0','5.0.1','5.1.0','5.2.0','5.3.0','5.3.1','5.3.2','5.4.0','5.5.0','5.6.0','5.7.0',
         '5.8.0','5.9.0','6.0.0','6.0.1','6.1.0','6.1.1','6.2.0'],
    msom: ['5.3.0','5.3.1','5.3.2','5.4.0','5.5.0','5.6.0','5.7.0','5.8.0','5.9.0','6.0.0',
           '6.0.1','6.1.0','6.1.1','6.2.0'],
    photon2: ['5.3.0','5.3.1','5.3.2','5.4.0','5.5.0','5.6.0','5.7.0','5.8.0','5.9.0',
              '6.0.0','6.0.1','6.1.0','6.1.1','6.2.0']
  };

  var vInfo = {versions:[]};
  for (var i=0; i<legacyVersions.length; i++) {
    var pv = apiHelper.parseVersionStr(legacyVersions[i]);
    var sv = (pv.major * 65536) + (pv.minor * 256) + (pv.patch || 0);
    if (pv.preAdj) sv -= pv.preAdj;
    vInfo.versions.push({semVer:legacyVersions[i], sys:sv});
  }

  apiHelper.getRestoreVersions = function(d) {
    for (var i=0; i<platforms.length; i++) {
      var p = platforms[i];
      if (p.id === d.platformId && verByPf[p.name]) {
        var r = Object.assign({}, p);
        r.versionArray = verByPf[p.name];
        r.isTracker = (r.id === 26 || r.id === 28);
        r.hasNCP = (r.id === 26) || (r.id === 12);
        r.isRTL872x = r.mcu && r.mcu.startsWith('RTL872');
        r.isnRF52 = r.mcu && r.mcu.startsWith('nRF52');
        r.isSTM32F2xx = r.mcu && r.mcu.startsWith('STM32F2');
        return r;
      }
    }
    return null;
  };

  apiHelper.systemVersionToSemVer = function(sv) {
    for (var i=0; i<vInfo.versions.length; i++) { if (vInfo.versions[i].sys === sv) return vInfo.versions[i].semVer; }
    return null;
  };
  apiHelper.semVerToSystemVersion = function(sv) {
    for (var i=0; i<vInfo.versions.length; i++) { if (vInfo.versions[i].semVer === sv) return vInfo.versions[i].sys; }
    return 0;
  };
  apiHelper.platformIdToName = function(id) {
    for (var i=0; i<platforms.length; i++) { if (platforms[i].id === id) return platforms[i].name; }
    return null;
  };

  // ---- Stubs for auth/cloud (WiFi mode skips all of these) ----
  apiHelper.auth = null;
  apiHelper.loginAgain = function(){};
  apiHelper.particle = {
    getDevice: function(){ return Promise.resolve({body:{}}); },
    listDevices: function(){ return Promise.resolve({body:[]}); },
    callFunction: function(){ return Promise.resolve({}); },
    getEventStream: function(){ return Promise.resolve({}); },
    getUserInfo: function(){ return Promise.resolve({}); },
    activateSIM: function(){ return Promise.resolve({}); },
    addDeviceToProduct: function(){ return Promise.resolve({}); },
    markAsDevelopmentDevice: function(){ return Promise.resolve({}); },
    listSIMs: function(){ return Promise.resolve({body:{sims:[]}}); }
  };
  apiHelper.selectedOrg = null;

  // ---- getDeviceConstants / getCarriersJson (used to build USB filter) ----
  // Returns device constants for all known Particle platforms
  apiHelper.getCarriersJson = function() {
    return Promise.resolve({
      deviceConstants: {
        argon: {id:12,name:'argon',generation:3,wifi:true,cellular:false},
        boron: {id:13,name:'boron',generation:3,wifi:false,cellular:true},
        bsom: {id:23,name:'bsom',generation:3,wifi:false,cellular:true},
        b5som: {id:25,name:'b5som',generation:3,wifi:false,cellular:true},
        tracker: {id:26,name:'tracker',generation:3,wifi:false,cellular:true},
        trackerm: {id:28,name:'trackerm',generation:3,wifi:false,cellular:true},
        p2: {id:32,name:'p2',generation:3,wifi:true,cellular:false},
        msom: {id:33,name:'msom',generation:3,wifi:true,cellular:true},
        photon2: {id:34,name:'photon2',generation:3,wifi:true,cellular:false},
        monitorm: {id:35,name:'monitorm',generation:3,wifi:false,cellular:true},
        electron2: {id:37,name:'electron2',generation:3,wifi:false,cellular:true},
        photon: {id:8,name:'photon',generation:2,wifi:true,cellular:false},
        electron: {id:10,name:'electron',generation:2,wifi:false,cellular:true},
        bseries: {id:14,name:'bseries',generation:2,wifi:false,cellular:true},
        core: {id:0,name:'core',generation:1,wifi:false,cellular:false}
      },
      skus: []
    });
  };
  apiHelper.getDeviceConstants = function() {
    return apiHelper.getCarriersJson().then(function(r){return r ? r.deviceConstants : null;});
  };
})();