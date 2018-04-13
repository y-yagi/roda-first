= Rodaについて知ろう

== Rodaとは

Rodaは、Jeremy Evans(@jeremyevans)@<fn>{jeremyevans}氏によって作られたライブラリです。Jeremy Evans氏は、データベースツールキットのSequel@<fn>{sequel}や、テンプレートエンジンのErubi@<fn>{erubi}の作者でもあります。特にErubiは、Rails 5.1からRails標準のテンプレートエンジンになっており、名前を聞いた事ある方も多いかもしれません。
//footnote[jeremyevans][https://github.com/jeremyevans]
//footnote[sequel][http://sequel.jeremyevans.net/]
//footnote[erubi][https://github.com/jeremyevans/erubi]

最初のバージョン(2014/07/30リリース)から現在まで、RodaはJeremy Evans氏個人によって開発が続けられています。

HP@<fn>{hp}やソースコードのREADME@<fn>{readme}では、Rodaの事を"Roda is a routing tree web toolkit"と紹介しています。
//footnote[hp][http://roda.jeremyevans.net/]
//footnote[readme][https://github.com/jeremyevans/roda/blob/master/README.rdoc]

この紹介の通り、Rodaはルーティング機能を提供するライブラリです。Model-View-Controller(以降MVC)パターンにおけるController部分の機能のみを提供しており、RailsのようにMVC全ての機能を提供しているわけではありません。

Ruby製で同様のライブラリだと、Sinatra@<fn>{sinatra}があります。実際、RodaはSinatraを参考に作られており、HP上にはSinatraとの比較についての説明が記載されています。
//footnote[sinatra][http://sinatrarb.com/]

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
//footnote[cuba][http://roda.jeremyevans.net/]
//footnote[fork][RodaのGitHubのリポジトリをみると、"forked from soveran/cuba"と表示されているのが確認できます]

Rodaは、CubaとSinatraという二つの異なるライブラリの思想や設計に影響を受けて、今のような形になっています。


== プラグインシステム

Rodaの大きな特徴の一つに、プラグインシステムがあります。Rodaは、各種機能をプラグインとして提供するようにして、コアの機能は最小限になるようにしています。例えばテンプレートのレンダリング処理もデフォルトでは使用出来るようになっておらず、明示的にプラグインを利用する必要があります。

プラグインは@<code>{plugin}メソッドを対象のプラグインを読む込みことで出来るようになっています。先ほどのコード例だと、@<code>{plugin :all_verbs}がそうです。@<code>{all_verbs}プラグイン使用する事により、@<code>{patch}や@<code>{delete}のようなHTTPリクエストメソッドと同じ名前のメソッドが使用出来るようになります。

これはRailsの原則の一つである"おまかせ"とは真逆で、使用したい料理を自分で明示的に選択する必要があります。そもそもどんな料理があるかがわからない最初のうちは、これは大変な作業かもしれません。

しかしこのシステムにより、不要な機能が読み込まれず、結果ライブラリの動作が高速になったり、何が問題があった際に確認すべきソースが少なくなったりと、少なくない恩恵があります。また、何か自分が独自に機能を追加したい場合も、プラグインとしてその機能を提供することで、簡単に機能を追加出来る、というメリットもあります。


== Faster
