= Rodaについて知ろう

== Rodaとは

Rodaは、Jeremy Evans(@jeremyevans)@<fn>{jeremyevans}氏によって作られたライブラリです。Jeremy Evans氏は、データベースツールキットのSequel@<fn>{sequel}や、テンプレートエンジンのErubi@<fn>{erubi}の作者でもあります。特にErubiは、Rails 5.1からRails標準のテンプレートエンジンになっており、名前を聞いた事ある方も多いかもしれません。
//footnote[jeremyevans][@<href>{https://github.com/jeremyevans}]
//footnote[sequel][RailsにおけるActive Record相当のライブラリ。O/Rマッパーやデータベースのマイグレーションの為の機能が提供されている。http://sequel.jeremyevans.net/]
//footnote[erubi][ERBテンプレートを表示する為のテンプレートエンジン。https://github.com/jeremyevans/erubi]

最初のバージョン(2014/07/30リリース)から現在まで、RodaはJeremy Evans氏個人によって開発が続けられています。

HP@<fn>{hp}やソースコードのREADME@<fn>{readme}では、Rodaの事を"Roda is a routing tree web toolkit"と紹介しています。
//footnote[hp][@<href>{http://roda.jeremyevans.net/}]
//footnote[readme][@<href>{https://github.com/jeremyevans/roda/blob/master/README.rdoc}]

この紹介の通り、Rodaはルーティング機能を提供するライブラリです。Model-View-Controller(以降MVC)パターンにおけるController部分の機能のみを提供しており、RailsのようにMVC全ての機能を提供しているわけではありません。

Ruby製で同様のライブラリだと、Sinatra@<fn>{sinatra}があります。実際、RodaはSinatraを参考に作られており、HP上にはSinatraとの比較についての説明が記載されています。SinatraとRodaの違いについては、@<hd>{SinatraとRoda}で触れたいと思います。
//footnote[sinatra][@<href>{http://sinatrarb.com/}]

== Hello World!

違いについて説明する前に、まずはRodaにさわってみましょう。ここでは、よくある「Hello, World!」と表示するだけのウェブアプリケーションを作ろうと思います。

まずは、Rodaをインストールしましょう。なお、執筆時点で最新のバージョンである3.6.0を使用します。

//cmd{
$ gem install roda
Successfully installed roda-3.6.0
1 gem installed
//}

これで準備は完了です。では、実際にRodaを使ってみましょう。

RodaはRackベースのライブラリです。そのため、RodaでWebアプリケーションを作るには、Rackアプリを起動する為の設定ファイルが必要になるため、その設定ファイルを作成しましょう。ファイル名は、デフォルトのファイル名である"config.ru"という名前にします。

//list[config.ru][config.ru]{
#@mapfile(scripts/01/hello_world_1.rb)
require "roda"

class App < Roda
  route do |r|
    r.root do
      "Hello World!"
    end
  end
end

run App.freeze.app
//}

ファイルが作成出来たら、ファイルがあるのと同じディレクトリでrackupコマンドを実行して、アプリケーションを起動してみましょう。

//cmd{
$ rackup
//}


最後に、ブラウザを起動し、@<href>{http://localhost:9292/}にアクセスしてくみてください。"Hello World!"と表示されれば成功です。

コードについては、恐らくクラス名やメソッド名から何をしている処理なのか、大体わかってしまうかもしれませんが、簡単に説明をしたいと思います。

: Roda
  Rodaのコアとなるクラス。このクラスを継承したクラスで処理を定義す必要があります。
: route
  ここで定義したブロックがリクエストが来た際にに実行されます。ブロックはRack::Requestのサブクラスのインスタンスで実行されます。
: root
  リクエストのパスが@<i>{/}、かつ、@<i>{GET}リクエストの場合に実行されるブロック。
: freeze
  アプリケーションの設定やミドルウェアをfreezeする為のメソッド。freezeしておくことで、実行時に設定やミドルウェアが予期せず変更されてしまうことを検知出来ます。プロダクションやテスト環境では、freezeしておくことが推奨されています。


== SinatraとRoda

色々と違いあるのですが、一番大きな違いとしてルーティングの書き方の違いがあげれます。

Sinatraはルーティングををフラットなリストで定義するのに対して、Rodaは木構造で定義します。

//list[sinatra][sinatra]{
#@mapfile(scripts/01/sinatra.rb)
require 'sinatra/base'

class App < Sinatra::Base
  get '/' do
    # TOPページ
  end

  get '/todos/:id' do
    @todo = Todo[params[:id].to_i]
    # TODO表示
  end

  post '/todos/:id' do
    @todo = Todo[params[:id].to_i]
    # TODO更新
  end

  delete '/todos/:id' do
    @todo = Todo[params[:id].to_i]
    # TODO削除
  end
end
#@end
//}

//list[roda][roda]{
#@mapfile(scripts/01/roda.rb)
require 'roda'

class App < Roda
  plugin :all_verbs
  route do |r|
    r.root do
      # TOPページ
    end

    r.is 'todos', Integer do |todo_id|
      @todo = Todo[params[:id]

      r.get do
        # TODO表示
      end

      r.post do
        # TODO更新
      end

      r.delete do
        # TODO削除
      end
    end
  end
end
#@end
//}


一見するとSinatraの方がURLと処理のマッピングわかりやすくて良いのでは、と思われる方もいると思います。それはそれで正しい意見だと思います。しかし、木構造になっている事で、ネームスペースや変数の定義をDRYにする事が出来ます。

これはパスが一階層しかないとあまり恩恵が無いかもしれませんが、パスの階層が深くなればなるだけ、よりわかりやすさに差が出てくると思います。

なお、ルーティングに木構造を使用するライブラリはRodaがはじめてではありません。Cuba@<fn>{cuba}というライブラリがこの方法をとっており、実際RodaはCubaをforkして開発されています@<fn>{fork}。
//footnote[cuba][@<href>{http://roda.jeremyevans.net/}]
//footnote[fork][RodaのGitHubのリポジトリをみると、"forked from soveran/cuba"と表示されているのが確認できます]

Rodaは、CubaとSinatraという二つの異なるライブラリの思想や設計に影響を受けて、今のような形になっています。


== プラグインシステム

Rodaの大きな特徴の一つに、プラグインシステムがあります。Rodaは、各種機能をプラグインとして提供するようにして、コアの機能は最小限になるようにしています。例えばテンプレートのレンダリング処理もデフォルトでは使用出来るようになっておらず、明示的にプラグインを利用する必要があります。

プラグインは@<code>{plugin}メソッドを対象のプラグインを読む込みことで出来るようになっています。先ほどのコード例だと、@<code>{plugin :all_verbs}がそうです。@<code>{all_verbs}プラグイン使用する事により、@<code>{patch}や@<code>{delete}のようなHTTPリクエストメソッドと同じ名前のメソッドが使用出来るようになります。

これはRailsの原則の一つである"おまかせ"とは真逆で、使用したい料理を自分で明示的に選択する必要があります。そもそもどんな料理があるかがわからない最初のうちは、これは大変な作業かもしれません。

しかしこのシステムにより、不要な機能が読み込まれず、結果ライブラリの動作が高速になったり、何が問題があった際に確認すべきソースが少なくなったりと、少なくない恩恵があります。また、何か自分が独自に機能を追加したい場合も、プラグインとしてその機能を提供することで、簡単に機能を追加出来る、というメリットもあります。

なお、Roda本体で提供しているプラグインについては、doc@{fn}{document}にまとまっています。Roda本体だけでどういう機能が提供されているか気になる方は、一度チェックしてみてください。
//footnote[document][@<href>{http://roda.jeremyevans.net/documentation.html#plugins}]


== パフォーマンス

RodaがHP上でうたっている特徴の一つに、パフォーマンスがあります。HP上には、@<i>{Roda is the one of the fastest Ruby web frameworks.}という記載もあります。中々強気な発言ですね。

では、実際のパフォーマンスなどうなのでしょうか。Roda、及び他のフレームワークのパフォーマンスについて触れているサイトがあるので、それを見てみましょう。

まずは、@<href>{https://github.com/luislavena/bench-micro} です。これは、RubyのWeb用のマイクロフレームワークについてのベンチマークを行っているリポジトリです。アプリは"Hello World!"を返すだけの非常にシンプルなアプリになっています。なお、ベンチマーキングには、wrk@<fn>{wrk}を使用しています。
//footnote[wrk][@<href>{https://github.com/wg/wrk}]

結果は下記の通りです。

//list[bench-micro][@<href>{https://github.com/luislavena/bench-micro}から引用]{
Requests/sec

Framework            Requests/sec  % from best
----------------------------------------------
rack                     15839.64      100.00%
watts                    13585.65       85.77%
syro                     13306.01       84.00%
roda                     11776.27       74.35%
cuba                     11501.96       72.62%
rack-response            11480.11       72.48%
hobbit                   11140.22       70.33%
hobby                    10606.80       66.96%
hanami-router            10247.61       64.70%
newark                    8896.35       56.17%
rambutan                  8466.83       53.45%
plezi                     8240.07       52.02%
rackstep                  8187.32       51.69%
rack-app                  7473.52       47.18%
rails-metal               6598.65       41.66%
flame                     5454.46       34.44%
sinatra                   3977.30       25.11%
grape                     2937.03       18.54%
rails-api                 1211.33        7.65%
//}

一位はRackです。昨今のフレームワークはRackをベースにしている為、素のRackが一番速いのは当然ですね。ついで、watts@<fn>{watts}、syro@<fn>{syro}ときて、roda、という順位なっています。
//footnote[watts][リソース指向のフレームワーク @<href>{https://github.com/pete/watts}]
//footnote[syro][ルータライブラリ @<href>{http://soveran.github.io/syro/}]

上位三つと比べると少々劣りますが、それでも十分速いのではないでしょうか。なお、比較対象にあげられているSinatraと比べると、3倍近い性能を出しています。

もう一つ、TechEmpower社がおこなっているWeb Framework Benchmarks(@<href>{https://www.techempower.com/benchmarks/})を見てみましょう。

これはRubyに限らず、様々なプログラミング言語のWebアプリケーションのフレームワークのパフォーマンス比較を行い、その結果を表示しているサイトです。ベンチマークは定期的に取得するようになっており、最新の結果(Round 15 / 2018-02-14)では、26のプログラミング言語の158ものフレームワークがベンチマークの対象になっています。

ベンチマークは複数のパターン(JSONを返す、データベースに対して一つSQLを実行する、等々)、データベースも複数パターン(MySQL、PostgreSQL、MongoDB、等々)、アプリケーションサーバも当然複数パターン(Puma、unicorn、等々)で結果を取得するようなっており、大変多岐に渡っています。

結果の全てを説明するのは紙面の都合上無理なので、ここではRubyの結果だけみていきましょう。なお、色々と面白い結果になっていると思うので、是非ともサイト上で結果の詳細を見てみることをおすすめします。

まずは、JSONレスポンスの結果です。

//image[tfb_ruby_json][TechEmpower Framework Benchmarks(Ruby/JSON)]{
//}

mrubyとCRubyの結果を混ぜるのはそもそもどうなのだろう、という意見もあるかと思いますが、そこは一旦置いておいておいてやはりmruby + HTTPサーバが圧倒的に速いですね。mrubyについで、CRubyではroda + Sequel + PostgreSQLの組み合わせが三位になっています。ついで四位もroda + Sequel(こちらはデータベースがMySQL)です。なお、どちらもアプリケーションサーバはPumaです。そしてその次にSinatra + PostgreSQLの組みあわせが来ています。先ほどのbench-microほどの差は出ていないですが、それでも1.7倍程度高速なようです。

もう一つ、データベースに対して一つSQLを実行した場合の結果も見てみましょう。

//image[tfb_ruby_single_query][TechEmpower Framework Benchmarks(Ruby/Single query)]{
//}

roda + Sequelの組み合わせが一位、二位という結果になりました。JSONの結果より差は出なくなってきていますが、やはりSinatraよりは速い、という結果になっています。 これらの結果を見る限り、@<i>{Roda is the one of the fastest Ruby web frameworks.}というのは、そこまで大げさではないように思えますね。


===[column] Rodaという名前について

Rodaという名前は、どういう意図でつけられたのでしょうか? 辞書を調べてみても、Rodaという単語は存在しません。では何かの頭文字をとってつけたのかというと、そうでは無いようです。

同じ疑問を持った方がいて、Hacker Newsの、Rodaに関するスレ(@<href>{https://news.ycombinator.com/item?id=8152403})に、質問が記載され、それに対してJeremy Evans氏本人からの回答があります。回答は下記の通りです。

@<i>{The name Roda comes from the Ys video game series, in which the Roda trees play a small role. I'm a huge Ys fan.}

(@<href>{https://news.ycombinator.com/item?id=8155027} から引用)

という訳で、日本ファルコムのイースというゲームに出てくるロダの樹から引用したとのことです。ロダの樹は巨大な二本の老木です。恐らくルーティングが木構造になっていることから、連想したのだと思われます。因みに、今は流石になくなりましたが、昔はRodaのHPにイースのゲーム画面のスクリーンショットが貼られていたりしました。

===[/column]
