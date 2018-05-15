var mmd;
window.onload = function() {
  var size = 512
  var canvas = document.createElement('canvas');
  canvas.width = size;
  canvas.height = size;
  canvas.style.border = 'solid black 1px';

  document.body.appendChild(canvas);

  mmd = new MMD(canvas, canvas.width, canvas.height);
  //var mmd = new MMD(canvas, canvas.width, canvas.height);
  mmd.initShaders();
  mmd.initParameters();
  mmd.registerKeyListener(document);
  mmd.registerMouseListener(document);
  
  //(IsebaLaboMaster追記コメントここから)
  //
  //下記ソースコード
  //「var miku = new MMD.Model('model', 'Miku_Hatsune_Ver2.pmd');」
  //のうち、「Miku_Hatsune_Ver2.pmd」のファイル名を以下のものに変更すると
  //MMDモデルを変更することができます。
  //
  //「Haku_Yowane.pmd」
  //「KAITO.pmd」
  //「Len_Kagamine.pmd」
  //「MEIKO.pmd」
  //「Meiko_Sakine.pmd」
  //「Miku_Hatsune.pmd」
  //「Miku_Hatsune_metal.pmd」
  //「Miku_Hatsune_Ver2.pmd」
  //「Neru_Akita.pmd」
  //「Rin_Kagamene_act2.pmd」
  //「Rin_Kagamine.pmd」
  //
  //※これら変更はMMDモデルデータの扱いおよびプログラムコード編集の知識がある方推奨です。
  //
  //(IsebaLaboMaster追記コメントここまで)
  var miku = new MMD.Model('model', 'Miku_Hatsune_Ver2.pmd');
  
  //var miku = new MMD.Model('Lat', 'Normal.pmd');
  //var miku = new MMD.Model('mobko', 'mobko.pmd');
  //var miku = new MMD.Model('yufu', 'yufu.pmd');
  //var miku = new MMD.Model('defoko', 'defoko.pmd');
  
  miku.load(function() {
    mmd.addModel(miku);
    mmd.initBuffers();
    mmd.start();

    var dance = new MMD.Motion('motion/kishimen.vmd');
    dance.load(function() {
      mmd.addModelMotion(miku, dance, true);

      mmd.play()
      
      

      setInterval(function() {
        console.log('fps = ' + mmd.realFps);
      }, 1000);
    });
  });
};
