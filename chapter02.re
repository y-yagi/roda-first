= Rodaでアプリケーションを作ってみよう

== 前準備

今回のアプリケーションではデータベースにはPostgreSQLを使用します。バージョンは9.2以上であればOKです。各自環境に合わせた方法でインストールをお願いします。

PostgreSQLのインストールが完了したら、アプリケーションで使用するデータベースとユーザを作成しましょう。下記コマンドをPostgreSQLのユーザで実行してください。

//cmd{
$ createuser -U postgres my_todo
$ createdb -U postgres -O my_todo my_todo_development
//}

これで前準備完了です。

== アプリケーション準備

実際にRodaを使って簡単なTodoアプリケーションを作ってみましょう。なお、サンプルのコードが https://github.com/y-yagi/my_todo にあります。必要に応じて参考にしてください。

RodaはRailsにおける@<code>{rails new}コマンドのようなスケルトンを作成する為の仕組みがありません。全て自分でセットアップする必要があります。

とはいえ、ゼロからセットアップをするのは、慣れないうちは大変です。幸い、作者であるJeremy Evans氏がRoda/Sequelを使用したアプリケーションを作成する為のスケルトンアプリケーションを公開してくれているので、今回はそちらを使用しようと思います。 なお、今回は、そのスケルトンに合わせてO/RマッパーにSequelを使用します。Sequelの使い方については、適時説明を入れていきますので、Sequelが使った事が無くても大丈夫です。

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
  Minitestでaround/around(:all) hooksを使用出来るようにする為のライブラリです。なお、Jeremy Evans氏は普段Minitest@<fn>{minitest}を使っているようで、このスケルトンアプリケーションでもMinitestを使うようになっています。
//footnote[minitest][RodaやSequel、Erubi自体のテストがMinitestで書かれています。]

@<code>{puma}を追加したら、@<code>{bundle install}でgemのインストールを行いましょう。

//cmd{
$ bundle install
//}

最後に、setup用のrakeタスクを実行します。このタスクでは、データベースの接続先や、cookieのキーを引数に指定されたアプリケーション名に書き換えてくれます。

//cmd{
$ rake "setup[MyTodo]"
//}

これで準備完了です。

では、アプリケーションを起動してみましょう。dev_upタスクでマイグレーションの実行、及び、rackupコメントでアプリケーションを起動してください。

//cmd{
$ rake dev_up
$ rackup
//}

http://localhost:9292/ にブラウザでアクセスして、"Hello World!"が表示されれば、起動成功です。


== アプリケーション構成

ここでは、スケルトンアプリケーションの構成についてみていきましょう。

ファイル構成は下記のようになっているかと思います。

//emlist{
.
├── Gemfile
├── Gemfile.lock
├── README.rdoc
├── Rakefile
├── app.rb
├── assets
│   └── css
│       └── app.scss
├── config.ru
├── db.rb
├── migrate
│   └── 001_tables.rb
├── models
│   └── model1.rb
├── models.rb
├── routes
│   └── prefix1.rb
├── spec
│   ├── minitest_helper.rb
│   ├── model
│   │   ├── model1_spec.rb
│   │   └── spec_helper.rb
│   └── web
│       ├── prefix1_spec.rb
│       └── spec_helper.rb
└── views
    ├── index.erb
    └── layout.erb
//}

主要なファイルについて詳細をみていきましょう。

=== Rakefile

まずは、Rakefileです。

//list[Rakefile][Rakefile]{
#@mapfile(scripts/02/Rakefile)
# Migrate

migrate = lambda do |env, version|
  ENV['RACK_ENV'] = env
  require_relative 'db'
  require 'logger'
  Sequel.extension :migration
  DB.loggers << Logger.new($stdout)
  Sequel::Migrator.apply(DB, 'migrate', version)
end

desc "Migrate test database to latest version"
task :test_up do
  migrate.call('test', nil)
end

desc "Migrate test database all the way down"
task :test_down do
  migrate.call('test', 0)
end

desc "Migrate test database all the way down and then back up"
task :test_bounce do
  migrate.call('test', 0)
  Sequel::Migrator.apply(DB, 'migrate')
end

desc "Migrate development database to latest version"
task :dev_up do
  migrate.call('development', nil)
end

desc "Migrate development database to all the way down"
task :dev_down do
  migrate.call('development', 0)
end

desc "Migrate development database all the way down and then back up"
task :dev_bounce do
  migrate.call('development', 0)
  Sequel::Migrator.apply(DB, 'migrate')
end

desc "Migrate production database to latest version"
task :prod_up do
  migrate.call('production', nil)
end

# Shell

irb = proc do |env|
  ENV['RACK_ENV'] = env
  trap('INT', "IGNORE")
  dir, base = File.split(FileUtils::RUBY)
  cmd = if base.sub!(/\Aruby/, 'irb')
    File.join(dir, base)
  else
    "#{FileUtils::RUBY} -S irb"
  end
  sh "#{cmd} -r ./models"
end

desc "Open irb shell in test mode"
task :test_irb do
  irb.call('test')
end

desc "Open irb shell in development mode"
task :dev_irb do
  irb.call('development')
end

desc "Open irb shell in production mode"
task :prod_irb do
  irb.call('production')
end

# Specs

spec = proc do |pattern|
  sh "#{FileUtils::RUBY} -e 'ARGV.each{|f| require f}' #{pattern}"
end

desc "Run all specs"
task default: [:model_spec, :web_spec]

desc "Run model specs"
task :model_spec do
  spec.call('./spec/model/*_spec.rb')
end

desc "Run web specs"
task :web_spec do
  spec.call('./spec/web/*_spec.rb')
end
//}

Rakefileには、大きく分けて三つのタスクが定義されています。データベースマイグレーション用のタスク、コンソール用のタスク、および、テスト用のタスクです。

まず、データベースマイグレーション用のタスクです。Sequelでは、データベースマイグレーション用のRakeタスク(Railsにおける@<code>{db:migrate}や@<code>{db:rollback}が提供されていない為、自前でタスクを準備しています。環境名 + up / downでタスクが実行出来るようになっています。例えば、development環境のmigrateを実行したい場合は、@<code>{dev_up}を実行すればOKです。なお、downタスクは、@<code>{db:rollback}と異なり、最初のマイグレーションまでrollbackされてしまうので、その点だけご注意下さい。

次に、コンソール用のタスクです。コードを見て頂くとわかるとおり、基本的はirbを実行しているだけです。RACK_ENVを指定しているのと、models配下のファイルを読み込むようになっているので、irb起動後、すぐmodelが使用出来るようになっています。

最後に、テスト用のタスクです。model用のテストとweb(Capybaraを使ったテスト)用のテストでタスクが別れています。なお、"spec"という名前が少し紛らわしいかもしれませんが、Gemfileの箇所でも説明した通り、テストはMinitestを使うようになっています。MinitestのBDD風にテストを書くためのライブラリである、minitest/specを使っている為、"spec"という名前になっています。

今回は紙面の都合上、テストについての説明は省いていますが、RodaもSequelもテストの為の特別な機能を提供している訳ではないので、Minitest、Capybaraの機能だけを使用してテストは書かれています。そのため、MinitestをRSpecに置き換えても、特に問題無くテストは書けます。

=== config.ru

続いています、config.ruです。

最初に書いたHello Worldを表示するアプリケーションでは、config.ruにメインとなるコードを記載しましたが、このスケルトンアプリケーションでは、メインのコードはapp.rbに記載するようにして、config.ruではそのファイルを読み込むようになっています。

//list[config.ru][config.ru]{
#@mapfile(scripts/02/config.ru)
dev = ENV['RACK_ENV'] == 'development'

if dev
  require 'logger'
  logger = Logger.new($stdout)
end

require 'rack/unreloader'
Unreloader = Rack::Unreloader.new(subclasses: %w'Roda Sequel::Model',
                                  logger: logger, reload: dev){ MyTodo }
require_relative 'models'
Unreloader.require('app.rb'){ 'MyTodo' }
run(dev ? Unreloader : MyTodo.freeze.app)
//}

ここでは、development環境用に特別な処理として、STDOUTにログを出すようにしているのと、rack-unreloaderを使ってコードが変更されたら再読み込みされるようにしています。

=== app.rb

最後に、app.rbをいき見てましょう。このファイルにメインの処理が記載されています。

//list[app.rb][app.rb]{
#@mapfile(scripts/02/app.rb)
require_relative 'models'

require 'roda'
require 'tilt/sass'

class MyTodo < Roda
  plugin :default_headers,
    'Content-Type'=>'text/html',
    'Content-Security-Policy'=>"default-src 'self';
      style-src 'self' https://maxcdn.bootstrapcdn.com;",
    #'Strict-Transport-Security'=>'max-age=16070400;',
    # Uncomment if only allowing https:// access
    'X-Frame-Options'=>'deny',
    'X-Content-Type-Options'=>'nosniff',
    'X-XSS-Protection'=>'1; mode=block'

  # Don't delete session secret from environment in development mode as it breaks reloading
  session_secret = ENV['RACK_ENV'] == 'development' ? ENV['MY_TODO_SESSION_SECRET']
    : ENV.delete('MY_TODO_SESSION_SECRET')
  use Rack::Session::Cookie,
    key: '_MyTodo_session',
    #secure: ENV['RACK_ENV'] != 'test', # Uncomment if only allowing https:// access
    :same_site=>:lax, # or :strict if you want to disallow linking into the site
    secret: (session_secret || SecureRandom.hex(40))

  plugin :csrf
  plugin :flash
  plugin :assets, css: 'app.scss', css_opts: {style: :compressed, cache: false},
                  timestamp_paths: true
  plugin :render, escape: true
  plugin :multi_route

  Unreloader.require('routes'){}

  route do |r|
    r.assets
    r.multi_route

    r.root do
      view 'index'
    end
  end
end
//}

まずは、default_headersプラグインを使用してヘッダーを指定しています。指定しているのは、Content-Security-PolicyやX-Frame-Options等の、主にセキュリティに関するヘッダーです。

次に、sessionの指定、及び、cookieを使用出来るようにする為に、Rack::Session::Cookie ミドルウェアをRackのスタックに追加しています。

次に、各種プラグインのロード処理です。csrfはCSRF対策、assetsはassetファイル(CSS及びJavaScript)のレンダリング、renderはテンプレートのレンダリングの為のプラグインです。

flashは、リクエスト間でデータを保持する為の仕組みです。Railsのflash@<fn>{flash}と同様の機能です。
//footnote[flash][http://api.rubyonrails.org/classes/ActionDispatch/Flash.html]

multi_routeは、複数の名前付きルートを作成出来るようにするプラグインです。通常、ルーティングの大本の定義である@<code>{root}は一つしか定義出来ません。しかし、multi_routeプラグインを使用する事により、この@<code>{root}を複数定義するように出来ます。これにより、例えば、APIに関するルーティングは別ファイルにする、ということが可能になっています。このスケルトンアプリケーションでは、routesディレクトリ配下にルーティングファイルが配置されるようになっています。そのため、その次の行で、rack-unreloaderでroutesディレクトリのファイルがreload対象になるようにしています。

@<code>{route}からが実際のルーティングの定義です。まず、@<code>{assets}、及び、@<code>{multi_route}メソッドを呼び出して、assetファイルのレンダリング、複数routeの読み込みが行われるようにしています。

最後に、@<code>{view 'index'}で、トップページにアクセスした際に、@<code>{index}テンプレートの中身が表示されるようにしています。

== テーブルの作成

ここでは、Todoを格納する為のテーブルを作成しましょう。

その前に、サンプルとして定義されているテーブルはもう不要なので、先にdev_downタスクを使ってテーブルを削除してしまいしょう。

//cmd{
$ rake dev_down
//}

また、models/model1.rb も不要です。こちらも合わせて削除してしまいしょう。

//cmd{
$ rm models/model1.rb
//}

では改めて、Todoを格納する為のテーブルを作成しましょう。

Todoの内容と、締切日を設定出来るテーブルを作成したいと思います。 migrate/001_tables.rb を、下記の内容に変更してください。元々の記載されいる内容は全て削除してしまって大丈夫です。

//list[001_tables.rb][001_tables.rb]{
Sequel.migration do
  change do
    create_table(:todos) do
      primary_key :id
      String :content, null: false
      DateTime :deadline
    end
  end
end
//}

Railsのマイグレーションに慣れている方であれば、恐らく見ただけで何をしているか何となくわかるかとは思いますが、簡単に説明したいと思います。

まず、Sequelのマイグレーションは全て@<code>{Sequel.migration}ブロックの中で実行する必要があるので、@<code>{Sequel.migration}を使用しています。

次に、up / downどちらで実行されるかを定義します。ただ、リバーシブルなマイグレーションであれば、changeを使うことで、自動でup / down時に適切なメソッドを使用するようになります。create_tableはリバーシブルなメソッド(downではdrop_tableが呼ばれる)為、ここではchangeを使用します。

最後に、create_tableを使用して実際に作成したテーブルを作成します。create_tableブロックの中でカラムを定義します。カラムは、型、カラム名、オプションという順番で定義します。

これでテーブル作成ようのマイグレーションファイルが出来たので、再度rev_upタスクを実行します。

//cmd{
$ rake dev_up
//}

テーブルが出来たので、そのテーブルを操作する為のmodelも準備します。modelsディレクトリ配下に、todo.rbというファイル名でmodelを作成します。ファイルの中身は下記の通りです。

//list[todo.rb][todo.rb]{
class Todo < Sequel::Model
end
//}

親クラスが@<code>{Sequel::Model}になっているだけで、Active Recordを使っているのと変わらないですね。

これでテーブル、及び、そのテーブルを操作する為のmodelの準備が出来ました。次に進みましょう。

== 画面の作成

ここから実際にTood機能を作っていきます。今回作成するアプリケーションでは画面は一ページだけで、一ページで、Toodの表示・作成を出来るようにしたいと思います。

Todoの表示からいきたいと思います。

まずは、サーバ側のコードから対応していきましょう。app.rbを編集して、トップページでTodoの一覧を表示出来るようにします。

//list[app.rb][app.rb]{
r.root do
  @todos = Todo.all
  view 'index'
end
//}

SequelはActive Record同様に@<code>{all}メソッドで全ての値を取得出来ます。これでTodoの一覧を取得出来るようになったので、view側でその値を表示するようにしましょう。

//list[views/index.erb][view/index.rb]{
<h1>Todo</h1>

<table class='table'>
  <thead>
    <tr>
      <th>内容</th>
      <th>期限</th>
      <th></th>
    </tr>
  </thead>

  <tbody>
    <% @todos.each do |todo| %>
      <tr>
        <td><%= todo.content %></td>
        <td><%= todo.deadline %></td>
      </tr>
    <% end %>
  </tbody>
</table>
//}

普通にHTMLですね。これで一覧の表示が出来るようになりました。

続いて、Todoの登録を出来るようにしましょう。こちらもサーバ側のコードから修正しましょう。

"/todos"に対してPOSTメソッドでデータが送信されたら、送信されたデータをTodoとして登録するようにします。@<code>{t.root}配下に、下記コードを追加してください。

//list[app.rb][app.rb]{
r.post 'todos' do
  Todo.create(r.params['todo'])
  flash[:notice] = 'Todoを作成しました'
  r.redirect '/'
end
//}

続けてview側です。views/index.erbの一番下に、下記コードを追加してください。

//list[views/index.erb][view/index.rb]{
<hr>

<h4>登録</h4>
<form action='/todos' method='post'>
  <%== csrf_tag %>
  <div class='form-group'>
    <input type='text' name='todo[content]' id='todo_content' required='true', placeholder='内容'>
    <input type='datetime-local' name='todo[deadline]' id='todo_deadline'>
  </div>

  <input type='submit' name='commit' value='作成', class='btn btn-primary'>
</form>
//}

RodaにはRailsのようなviewヘルパーメソッドはありません@<fn>{form}。そのため、フォーム等も普通にHTMLを記載する必要があります。
//footnote[form][Jeremy Evans氏がform用のライブラリを作成しており、そちらを使用すると、メソッドを使用してformを作成する事も可能です。 https://github.com/jeremyevans/forme]

ただのHTMLなので、特に説明はいらないかと思います。一点、CSRF対策用のタグを@<code>{csrf_tag}メソッドを使用して生成しているので、そこだけ注意してください。

最後に、Todoの削除を出来るようにしましょう。

Railsであれば、@<code>{link_to 'Destroy', xxx, method: :delete}でおしまい、なのですが、残念ながらviewヘルパーメソッドがないRodaではそんな簡単にはいきません。多少手間であるのですが、削除用のviewを追加し、そのページに削除ボタンを表示する、というアプローチをとりたいと思います。まずはサーバ側です。

//list[app.rb][app.rb]{
r.on 'todo', Integer do |id|
  @todo = Todo[id]

  r.get do
    view 'delete'
  end

  r.post 'destroy' do
    @todo.destroy
    flash[:notice] = 'Todoを削除しました'
    r.redirect '/'
  end
end
//}

削除ページの表示、及び、実際に削除する処理の二つルーティングを追加しています。どちらも/todo/:idというパスから始まるようにするために、まず@<code>{on}メソッドでブランチ(ここから"todo"パスのルーティングになります、という宣言のようなもの)を作成します。また、パラメータとしてTodoのIDが必須になるので、パラメータにIntegerの値がくることを宣言するように、@<code>{on}メソッドの引数に@<code>{Integer}を指定しています。

あとは、/todo/:idにGETメソッドでアクセスしたら削除のページを表示するよう、/todo/:id/destroyにPOSTメソッドでアクセスしたら実際に削除処理を行うようにそれぞれしています。サーバ側の処理はこれで完了です。

続けてview側です。まずは、views/index.erbに削除ページへのリンクを追加したいと思います。Todoの一覧を表示しているtableに、以下のようにaタグを追加してください。

//list[views/index.erb][view/index.rb]{
<tbody>
  <% @todos.each do |todo| %>
    <tr>
      <td><%= todo.content %></td>
      <td><%= todo.deadline %></td>
      <td><a href='<%= "/todo/#{todo.id}" %>'>削除</a></td>
    </tr>
  <% end %>
</tbody>
//}

最後に、削除ページのviewを追加します。delete.erbという名前のファイルを、viewディレクトリ配下に追加してください。

//list[views/delete.erb][view/delete.rb]{
<h4>削除</h4>
<form action=' <%= "/todo/#{@todo.id}/destroy" %>' method='post'>
  <%== csrf_tag %>
  <table class="table table-bordered table-striped">
    <tr class="string required">
      <td><label class="label-before" for="todo_content">内容</label></td>
      <td><span><%= @todo.content %></span></td>
    </tr>
    <tr class="string required">
      <td><label class="label-before" for="todo_deadline">期限</label></td>
      <td><span><%= @todo.deadline %></span></td>
    </tr>
  </table>
  <input type='submit' name='delete' value='削除', class='btn btn-danger'>
</form>
//}

これで削除も出来るようになりました。お疲れ様でした。
