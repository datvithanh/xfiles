#! /usr/bin/env bats

export PATH=$PATH:../../bin/

@test "Basic test 1" {
	t1="$(xtail test1.txt)"
	t2="$(tail test1.txt)"
	[[ $t1 == $t2 ]]
}

@test "Basic test 2" {
	t1="$(xtail test2.txt)"
	t2="$(tail test2.txt)"
	[[ $t1 == $t2 ]]
}

@test "Verbose test 1" {
	t1="$(xtail -v test1.txt)"
	t2="$(tail -v test1.txt)"
	[[ $t1 == $t2 ]]
}

@test "Verbose test 2" {
	t1="$(xtail test1.txt test2.txt)"
	t2="$(tail test1.txt test2.txt)"
	[[ $t1 == $t2 ]]
}

@test "Lines test 1" {
	t1="$(xtail -n 15 test1.txt)"
	t2="$(tail -n 15 test1.txt)"
	[[ $t1 == $t2 ]]
}

@test "Lines test 2" {
	t1="$(xtail -n 150 test2.txt)"
	t2="$(tail -n 150 test2.txt)"
	[[ $t1 == $t2 ]]
}

@test "Quiet test" {
	t1="$(xtail -q test1.txt test2.txt)"
	t2="$(tail -q test1.txt test2.txt)"
	[[ $t1 == $t2 ]]
}

@test "Overflow test" {
	t1="$(xtail -n 150 test1.txt)"
	t2="$(tail -n 150 test1.txt)"
	[[ $t1 == $t2 ]]
}
