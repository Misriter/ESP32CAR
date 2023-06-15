// pages/map/map.js
Page({

  /**
   * 页面的初始数据
   */
  data: {
    longitude:"",
    latitude:"",
    address:"",
    placename:"",
    isLogin:false,
    str:"",
    open:true,
    mode:true,
    joy_x:280,
    joy_y:600,
    imageX:'',
    imageY:'',
    joy_up:false,
    joy_down:false,
    joy_left:false,
    joy_right:false,

    button:{
      id:"",
      state:""
    }
  },
  joystick_move(e){
    var that=this
    var touchX = e.touches[0].clientX+70;
    var touchY = e.touches[0].clientY+160;
    var movePos = that.GetPosition(touchX, touchY);
    that.setData({
      joy_x:movePos.posX,
      joy_y:movePos.posY
    })
  },
  GetPosition: function (touchX, touchY) {
    var that = this;
    var DValue_X;
    var Dvalue_Y;
    var Dvalue_Z;
    var angle=0;
    DValue_X = touchX-280;
    Dvalue_Y = touchY-600;
    Dvalue_Z = (Math.sqrt(DValue_X * DValue_X + Dvalue_Y * Dvalue_Y));
    if(Dvalue_Y/DValue_X>0){
      if(DValue_X<0)
        angle=180+Math.atan(Dvalue_Y/DValue_X)*180/Math.PI
      else if(DValue_X>0)
        angle=Math.atan(Dvalue_Y/DValue_X)*180/Math.PI
    }
    else if(Dvalue_Y/DValue_X<0){
      if(Dvalue_Y<0)
        angle=360+Math.atan(Dvalue_Y/DValue_X)*180/Math.PI
      else if(DValue_X<0)
        angle=180+Math.atan(Dvalue_Y/DValue_X)*180/Math.PI
    }
    if(Dvalue_Z<100){
      that.setData({
        imageX:touchX,
        imageY:touchY,
      })
    }
    else{
      that.setData({
        imageX:380-(100-Math.cos(Math.PI*angle/180)*100),
        imageY:600+Math.sin(Math.PI*angle/180)*100,
      })
    }
    if(that.data.joy_up==false&&(that.data.imageY-600)<-95){
      that.setData({joy_up:true})
      this.up()
    }
    else if(that.data.joy_up==true&&(that.data.imageY-600)>-95){
      that.setData({joy_up:false})
      this.up()
    }
    if(that.data.joy_down==false&&(that.data.imageY-600)>95){
      that.setData({joy_down:true})
      this.down()
    }
    else if(that.data.joy_down==true&&(that.data.imageY-600)<95){
      that.setData({joy_down:false})
      this.down()
    }
    if(that.data.joy_left==false&&(that.data.imageX-280)<-95){
      that.setData({joy_left:true})
      this.left()
    }
    else if(that.data.joy_left==true&&(that.data.imageX-280)>-95){
      that.setData({joy_left:false})
      this.left()
    }
    if(that.data.joy_right==false&&(that.data.imageX-280)>95){
      that.setData({joy_right:true})
      this.right()
    }
    else if(that.data.joy_right==true&&(that.data.imageX-280)<95){
      that.setData({joy_right:false})
      this.right()
    }
    return { posX: that.data.imageX, posY: that.data.imageY };
  },
  joystick_return(e){
    var that=this
    that.setData({
      joy_x:280,
      joy_y:600
    })
    if(that.data.joy_up==true){
      that.setData({joy_up:false})
      this.up()
    }
    if(that.data.joy_down==true){
      that.setData({joy_down:false})
      this.down()
    }
    if(that.data.joy_left==true){
      that.setData({joy_left:false})
      this.left()
    }
    if(that.data.joy_right==true){
      that.setData({joy_right:false})
      this.right()
    }
  },
  switchMode:function(e){
    var that=this
    if(!that.data.isLogin){
      wx.showToast({
        title: '请先登录',
        icon: 'none',
        duration: 1500
      })
      return
    }
    that.setData({
      mode:!that.data.mode
    })
  },
  onChange:function(e){
    var that=this
    that.setData({
      open:!that.data.open
    })
  },
  location:function(){
    var that=this
    wx.getLocation({
      success:function (res) {
        console.log(res)
        that.setData
      }
    })
  },
  onLoad:function(e) {
    console.log("Ready")
  },
  onReady() {
  },
  onShow() {
  },
  onHide() {
  },
  onUnload() {
  },
  onPullDownRefresh() {
  },
  onReachBottom() {
  },
  onShareAppMessage() {
  },
  stringInput: function(e){
    var that=this
    this.setData({
        str:e.detail.value
    })
  },
  sendString: function(){
    var that = this
    if(!that.data.isLogin){
      wx.showToast({
        title: '请先登录',
        icon: 'none',
        duration: 1500
      })
      return
    }
    this.data.button.id="send"
    this.data.button.state=this.data.str
    wx.request({
      url: 'http://api.heclouds.com/cmds?device_id=973637421',
      header:{
        'content-type':'application/json',
        "api-key":"mUHm3YiEMOdGbRIR=uMRRbPMeyM="
      },
      method :"POST",
      data:this.data.button,
      success(res){
        console.log("控制成功")
        console.log(res)
        console.log(res.data);
        console.log(that.data.str);
      }
    })
  },
  move:function(e){
    var that = this
    if(!that.data.isLogin){
      wx.showToast({
        title: '请先登录',
        icon: 'none',
        duration: 1500
      })
      return
    }
    this.data.button.id="move"
    this.data.button.state=e.currentTarget.dataset.id
    wx.request({
      url: 'http://api.heclouds.com/cmds?device_id=973637421',
      header:{
        'content-type':'application/json',
        "api-key":"mUHm3YiEMOdGbRIR=uMRRbPMeyM="
      },
      method :"POST",
      data:this.data.button,
      success(res){
        console.log("控制成功")
        console.log(res)
        console.log(res.data);
      }
    })
  },
  stop:function(e){
    var that = this
    if(!that.data.isLogin){
      wx.showToast({
        title: '请先登录',
        icon: 'none',
        duration: 1500
      })
      return
    }
    this.data.button.id="move"
    this.data.button.state="stop"
    wx.request({
      url: 'http://api.heclouds.com/cmds?device_id=973637421',
      header:{
        'content-type':'application/json',
        "api-key":"mUHm3YiEMOdGbRIR=uMRRbPMeyM="
      },
      method :"POST",
      data:this.data.button,
      success(res){
        console.log("控制成功")
        console.log(res)
        console.log(res.data);
      }
    })
  },
  light:function(e){
    var that = this
    if(!that.data.isLogin){
      wx.showToast({
        title: '请先登录',
        icon: 'none',
        duration: 1500
      })
      return
    }
    this.data.button.id="button"
    this.data.button.state="light"
    wx.request({
      url: 'http://api.heclouds.com/cmds?device_id=973637421',
      header:{
        'content-type':'application/json',
        "api-key":"mUHm3YiEMOdGbRIR=uMRRbPMeyM="
      },
      method :"POST",
      data:this.data.button,
      success(res){
        console.log("控制成功")
        console.log(res)
        console.log(res.data);
      }
    })
  },
  playsound:function(e){
    var that=this
    var mp3 = e.currentTarget.dataset.mp3;
    if(!that.data.isLogin){
      wx.showToast({
        title: '请先登录',
        icon: 'none',
        duration: 1500
      })
      return
    }
    this.data.button.id="mp3"
    this.data.button.state=mp3
    wx.request({
      url: 'http://api.heclouds.com/cmds?device_id=973637421',
      header:{
        'content-type':'application/json',
        "api-key":"mUHm3YiEMOdGbRIR=uMRRbPMeyM="
      },
      method :"POST",
      data:this.data.button,
      success(res){
        console.log("控制成功")
        console.log(res)
        console.log(res.data);
      }
    })
  },
  login() {
    wx.getUserProfile({
      desc: "您是否同意获取您的用户信息",
      success: (res) => {
        this.setData({
          userInfo: res.userInfo,
          isLogin: true
        })
        console.log(res.userInfo)
        this.record()
      }
    })
  },
  record(){
    var that=this
    let datastream={
      "datastreams": [
        {"id": "last_user","datapoints":[{"value": that.data.userInfo.nickName}]},
      ]
    }
    wx.request({
      url: "https://api.heclouds.com/devices/973637421/datapoints",
      method:'POST',
      header:{
        "content-type": 'application/json',
        "api-key": "mUHm3YiEMOdGbRIR=uMRRbPMeyM="
      },
      data:JSON.stringify(datastream),
      success(res){
        console.log("更新数据成功",res)
      },
      fail: function(res){
        wx.showToast({ title: '系统错误' })
      },
    })
  },
  
})