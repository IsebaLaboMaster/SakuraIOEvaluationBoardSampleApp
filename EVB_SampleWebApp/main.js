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
  
  //(IsebaLaboMaster�ǋL�R�����g��������)
  //
  //���L�\�[�X�R�[�h
  //�uvar miku = new MMD.Model('model', 'Miku_Hatsune_Ver2.pmd');�v
  //�̂����A�uMiku_Hatsune_Ver2.pmd�v�̃t�@�C�������ȉ��̂��̂ɕύX�����
  //MMD���f����ύX���邱�Ƃ��ł��܂��B
  //
  //�uHaku_Yowane.pmd�v
  //�uKAITO.pmd�v
  //�uLen_Kagamine.pmd�v
  //�uMEIKO.pmd�v
  //�uMeiko_Sakine.pmd�v
  //�uMiku_Hatsune.pmd�v
  //�uMiku_Hatsune_metal.pmd�v
  //�uMiku_Hatsune_Ver2.pmd�v
  //�uNeru_Akita.pmd�v
  //�uRin_Kagamene_act2.pmd�v
  //�uRin_Kagamine.pmd�v
  //
  //�������ύX��MMD���f���f�[�^�̈�������уv���O�����R�[�h�ҏW�̒m��������������ł��B
  //
  //(IsebaLaboMaster�ǋL�R�����g�����܂�)
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
