= Rodaでアプリケーションを作ってみよう

== 前準備

今回のアプリケーションではDBにはPostgreSQLを使用します。バージョンは9.2以上であればOKです。各自環境に合わせた方法でインストールをお願いします。

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
//}

このスケルトンアプリケーションでは、アプリケーションサーバが指定されておらず、使用されるアプリケーションサーバがユーザの環境に依存するようになっています。今回はPumaを使用したいと思います。Gemfileの"rack-unreloader" gemの下に"puma"を追加してください。

//list[Gemfile][Gemfile]{
#@mapfile(scripts/02/Gemfile)
//}
