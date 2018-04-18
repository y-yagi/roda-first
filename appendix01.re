= Rodaの樹のもっと上へ

ここでは、Rodaについてもっと知る為のコンテンツについてご紹介します。

=== ドキュメント

Rodaのドキュメントは、 @<href>{http://roda.jeremyevans.net/documentation.html} にまとまっています。API docへのリンクだけではなく、プラグインの一覧や発表資料へのリンク等もまとまっていますので、まずはこのページを見るのがおすすめです。

=== ソースコード

詳細な挙動を確認したくなったら、やはりソースコードを見るのが確実です。Rodaのコアのファイル(roda.rb)は1000行程度しか無いので、割とすんなり読めると思います。

=== Giftsmas

ここからはRodaで作られたアプリケーションについてご紹介します。まずは、Giftsmas@<fn>{giftsmas}というアプリケーションです。これはJeremy Evans氏本人が作成したアプリケーションで、ギフトを管理する為のアプリケーション(どんなイベントがあって、誰に何のギフトを送ったのか等)です。
//footnote[giftsmas][@<href>{https://github.com/jeremyevans/giftsmas}]

modelが4つ、メインのコードも約250行程度しか無い小さなアプリケーションです。ですが、Jeremy Evans氏本人が作成しているということもあり、Roda及びSequelを使った実際のアプリケーションがどのようなものになるのかの参考になるでしょう。

=== Kontena

最後にもう1つ。Kontena@<fn>{kontena}という、Dockerコンテナを管理する為のアプリケーションです。
//footnote[kontena][@<href>{https://github.com/kontena/kontena/tree/master/server}]

こちらはRoda + DBにMongoDBという構成です。modelもroutesのファイルも40以上という中々重厚なアプリケーションです。中規模程度のアプリケーションをRodaを使って本当に作る事が出来るのかの参考になるでしょう。
