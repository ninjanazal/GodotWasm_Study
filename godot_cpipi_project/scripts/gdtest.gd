extends Node

func _ready():
	var a: int = 0;
	var initial_time = OS.get_ticks_msec();

	for i in range(20000000):
		a += i % 2;

	print(OS.get_ticks_msec() - initial_time);
	get_parent().call_test(get_name());
