ダウンロードありがとうございます。

このVSTプラグインは、ヒステリシスのPreisachモデルを使ったディストーションです。

ビルドするには、SteinbergのVST SDK Version 2.4が必要です。
SDKをビルドしてlibファイルを作ります。できたlibをこのプロジェクトのルートディレクトリに入れます。
SDKのincludeファイルをこのプロジェクトのvstディレクトリに入れます。
ディレクトリツリーはこんな感じになるはず。

hysteresisvst
	vst
		+aeffect.h
		+aeffectx.h
		+aeffeditor.h
		...
		
	+hysteresis.cpp
	+hysteresis.h
	+hysteresismain.cpp
	...
	+vstbase.lib
	+vstbased.lib


Ryogo Yoshimura (ryogo.yoshimura@gmail.com)
