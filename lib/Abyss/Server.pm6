use v6;
unit class Abyss::Server:ver<0.0.1>:auth<cpan:ANATOFUZ>;
use NativeCall;
use MONKEY-SEE-NO-EVAL;

sub dup(int32 $old) returns int32 is native { ... }
sub dup2(int32 $new, int32 $old) returns int32 is native { ... }
sub fork() returns int32 is native { ... }
sub waitpid(int32 $pid, int32 $wstatus is rw, int32 $option) returns int32 is native { ... }

method readeval {
    my $listen = IO::Socket::INET.new( :listen,
                                       :localhost<localhost>,
                                       :localport(3333), 
                                       :family(PF_INET));

    say DateTime.now;
    my $stdout = dup(1);
    my $stderr = dup(2);

    loop {
        # Receive the message. 
        my $conn = $listen.accept;
        my $buf = $conn.recv().chop;
        my @message = split("\n", $buf);

        my $dir = @message[0];
        chdir $dir;

        my $path = @message[1].IO;

        if fork() -> $pid {
            $conn.close;
            waitpid($pid, my int32 $wstatus, 0);
        } else {
            say "evaling "~$path;
            dup2($conn.native-descriptor(), 1);
            dup2($conn.native-descriptor(), 2);

            EVALFILE $path;

            exit(1);
        }
    }
    
    $listen.close;
}
