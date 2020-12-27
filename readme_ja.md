# catenary.aip

__catenary.aip__ は Adobe Illustrator 2020-2021 用のプラグインツールです。

[@Ushio](https://github.com/Ushio) さんのOpenFrameworks用ツールのリポジトリからforkしました。

## 使い方

![video-thumb](https://pbs.twimg.com/ext_tw_video_thumb/1226479325970460672/pu/img/vtfPc3LqW1PbP5CH?format=jpg&name=small)  
動画 - 
https://twitter.com/shspage/status/1226479347084615682

* ドラッグで曲線が描かれます。
* シフトキーを押しながらパスの端点をクリック、からのドラッグで、パスをカテナリー曲線として操作できます。（パスは一定の条件を持つ必要があります。後述。）
* シフトでなくオプションを押していると、パスを複製したうえで操作できます。
* ドラッグ中に上矢印を押すと曲線が短くなります。下矢印を押すと長くなります。 （曲線の下端を上下させると考えたほうが分かりやすいかもしれません。）

## 補足

シフトやオプションキーで操作対象にできるパスは以下の条件のものです。パスの長さは編集前のものが保持されます（曲線がたわんでいる場合。また、曲線として長すぎない場合。）

* 編集可能なオープンパス。塗りなし、線あり。
* いずれかの端点がパス全体の上端とほぼ同じ位置にある。
* ２つの端点がパス全体の左端、および右端とほぼ同じ位置にある。

## インストール

（ビルド後）
__catenary.aip__ をIllustratorのプラグインフォルダまたは追加プラグインフォルダに入れ、その後Illustratorを（再）実行してください。ツール一覧の一番下にある「アドオン」カテゴリにcatenaryがあると思います。

## 開発環境

Adobe Illustrator 2021 SDK  
Xcode 12.3 / macOS 11.0 sdk / macOS Big Sur

<!-- Visual Studio 2017 / Windows10 -->

## 補足（ビルド）
<!-- ソースコードは https://github.com/shspage/catenary_aip にあります。ビルドする際は以下をご一読ください。-->
* catenary_aip フォルダはIllustrator SDKのsamplecodeフォルダの直下に置いてください。
* (Windows) 添付のプロジェクトファイルでビルドするには、ソースコード(.cpp, .h, .hpp)の文字コードをUTF-8からMultibyte(cp932)に変換する必要があります。

## ライセンス

zlib License  
Copyright (c) 2020 Hiroyuki Sato  
https://github.com/shspage  
詳細は LICENSE-zlib.txt をご覧ください。


以下については、それぞれのライセンスを参照ください。

* catenary.hpp (namespace catenary)  
zlib License  
Copyright (c) 2017 ushiostarfish  
https://github.com/Ushio/Catenaly

* Adobe Illustrator 2020 SDK  
Copyright (c) 2020 Adobe. All rights reserved.  
https://www.adobe.io/


