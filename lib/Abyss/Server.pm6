use v6;
unit class Abyss::Server:ver<0.0.1>:auth<cpan:ANATOFUZ>;
use NativeCall;
use MONKEY-SEE-NO-EVAL;

sub fork() returns int32 is native { ... }
sub waitpid(int32 $pid, int32 $wstatus is rw, int32 $option) returns int32 is native { ... }

method readeval {
    my $listen = IO::Socket::INET.new( :listen,
                                       :localhost<localhost>,
                                       :localport(3333), 
                                       :family(PF_INET));

    say DateTime.now;

    loop {
        # Receive the message. 
        my $conn = $listen.accept;
        my $buf = $conn.recv().chop;
        my @message = split("\n", $buf);

        my $dir = @message[0];
        chdir $dir;

        my $path = @message[1].IO.absolute;

        my $in_file = @message[2];
        my $inh = open $in_file, :r;
        my $out_file = @message[3];
        my $outh = open $out_file, :a;
        my $err_file = @message[4];
        my $errh = open $err_file, :a;


        if fork() -> $pid {
            $conn.close;
            waitpid($pid, my int32 $wstatus, 0);
        } else {
            say "evaling "~$path;
            $*IN = $inh;
            $*OUT = $outh;
            $*ERR = $errh;

            EVALFILE $path;
            exit(1);
        }
    }
    
    $listen.close;
}
