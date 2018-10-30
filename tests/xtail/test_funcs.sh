#! /usr/bin/env bats

export PATH=$PATH:../../bin/

@test "Simple test 1" {
	t1="$(xtail test1.txt)"
	t2="$(tail test1.txt)"
	[[ $t1 == $t2 ]]
}

@test "Arg test 1" {
	t1="$(xtail -v test1.txt)"
	t2="$(tail -v test1.txt)"
	[[ $t1 == $t2 ]]
}

@test "Arg test 2" {
	t1="$(xtail -n 15 test1.txt)"
	t2="$(tail -n 15 test1.txt)"
	[[ $t1 == $t2 ]]
}
