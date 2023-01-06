use v6;
use lib $?FILE.IO.parent(2).add: 'lib';
use Abyss::Server;

my $abyss = Abyss::Server.new;
$abyss.readeval();
