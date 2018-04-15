= Rodaでアプリケーションを作ってみよう

== 前準備

今回のアプリケーションではDBにはPostgreSQLを使用します。バージョンは9.2以上であればOKです。各自環境に合わせた方法でインストールをお願いします。

PostgreSQLのインストールが完了したら、アプリケーションで使用するデータベースとユーザを作成しましょう。下記コマンドをPostgreSQLのユーザで実行してください。

//cmd{
$ createuser -U postgres my_todo
$ createdb -U postgres -O my_todo my_todo_development
//}

これで前準備完了です。


== アプリケーション準備

実際にRodaを使って簡単なTodoアプリケーションを作ってみましょう。

Todoアプリを作成するとなると、DBが必要になります。今回はPostgreSQLを使用しようと思います。

RodaはRailsにおける@<code>{rails new}コマンドのようなスケルトンを作成する為の仕組みがありません。全て自分でセットアップする必要があります。

とはいえ、ゼロからセットアップをするのは、慣れないうちは大変です。幸い、作者であるJeremy Evans氏がRoda/Sequelを使用したアプリケーションを作成する為のスケルトンアプリケーションを公開してくれているので、今回はそちらを使用しようと思います。

スケルトンアプリケーションは https://github.com/jeremyevans/roda-sequel-stack にありますので、まずは、git cloneしてアプリケーションをダウンロードしましょう。

//cmd{
$ git clone https://github.com/jeremyevans/roda-sequel-stack.git
//}

今回作成するアプリケーションは"MyTodo"という名前にしようと思いますので、ディレクトリ名も"my_todo"に変更しましょう。

//cmd{
$ mv roda-sequel-stack my_todo
$ cd my_todo
//}

このスケルトンアプリケーションでは、アプリケーションサーバが指定されておらず、使用されるアプリケーションサーバがユーザの環境に依存するようになっています。今回はPumaを使用したいと思います。Gemfileの@<code>{rack-unreloader}の下に@<code>{puma}を追加してください。

//list[Gemfile][Gemfile]{
#@mapfile(scripts/02/Gemfile)
source 'https://rubygems.org'

gem 'rack_csrf'
gem 'sass'
gem 'tilt', '>= 2'
gem 'erubi'
gem 'roda', '>= 3'
gem 'sequel', '>= 5'
gem 'sequel_pg'
gem 'rack-unreloader'
gem 'puma'

gem 'capybara'
gem 'minitest', '>= 5.7.0'
gem 'minitest-hooks', '>= 1.1.0'
//}

ついでなので、使用しているgemで、あまり普段使用しないであろうgemについて簡単に説明したいと思います。

: rack_csrf
  CSRF対策用Rackミドルウェア。Rodaのcsrfプラグインはこのライブラリを使用して実装されています。
: tilt
  各種テンプレートエンジンへの汎用インターフェイスを提供してくれるライブラリ。Rackのテンプレートレンダリングプラグインはこのライブラリを使用して実装されています。
: sequel_pg
  Sequelの行フェッチ処理をCで実装したライブラリ。これを使うことにより、素のSequelよりSELECT処理が高速に行えるようになっています。Jeremy Evans氏が作成した公式のライブラリです。なお、PostgreSQL版のみ存在し、MySQL版はありません。
: rack-unreloader
  アプリケーションのファイルが更新された際に、更新されたファイルを再読み込みしてくれるライブラリ。このライブラリにより、ファイルを変更した際にいちいちアプリケーションサーバを再起動する必要がなくなります。
: mintiest-hooks
  Minitestでbefore_all / after_all / around_all hooksを使用出来るようにする為のライブラリ。

@<code>{puma}を追加されたら、必要なgemがインストールされるよう、@<code>{bundle install}を実行してください。

//cmd{
$ bundle install
//}

最後に、setup用のrakeタスクを実行します。このタスクでは、DBの接続先や、cookieのキーを引数に指定されたアプリケーション名に書き換えてくれます。

//cmd{
$ rake "setup[MyTodo]"
//}

これで準備完了です。
