## Answers to Questions 1-3

**Q1**: What would be the characteristics of a program whose process would
	have a high nivctx number?

**A1**: Involuntary Context Switch --> Process gets preempted frequently.
	Such process must reach its time limit (quantum) frequently.
	Probably has low (or none) I/O dependencies and doesn't wait
	for user inputs. Does not call `sleep()`.

**Q2**: What would be a program whose process would have a high nvctx number?

**A1**: Volunraty Context Switch --> Process asks to be scheduled out before
	reaching time limit cause it can't make progress on it's job.
	Such process may wait for user input, handles I/O, may call `sleep()`.

**Q3**: What do you observe on the start time of some processes?
	Would you say that these are user programs, or not? Clarify your answer.

**A1**:

```
(...)
swapper/0,0,0,1,0,0,29157,0
-systemd,1,0,410,2,2668,3683,0
(...)
```

	swapper (PID=0) is the process that starts systemd (PID=1), indicated
	by aboot time of 0. These are not user programs.

	We can notice `kworker` instances with a boot time of 2 sec, these are
	probably kernel threads (indicated by the early start time and "k" prefix).

	Other early threads (with a boot time of 5-9 sec like `systemd-journal`,
	`systemd-oomd`, `dbus-daemon`, `NetworkManager`, etc.) are system daemons.
	Not user programs.

```
(...)
--systemd-journal,410,1,0,430,1389,313,5
--systemd-oomd,430,1,0,450,1095,85,5
(...)
--dbus-daemon,1186,1,0,1199,3114,891,8
(...)
--NetworkManager,1363,1,0,1371,499,689,9
```

	`test_k22tree` is clearly a user program. It's boot time is way past
	booting (1149 sec).

```
(...)
------test_k22tree,4495,3542,0,0,1,0,1149
(...)
```

**Conclusion**: low boot time indicates the process is a  kernel/system program.
		After all the initialization has happend - which takes time - user
		programs can be invoked. This means their boot time must be >> than system programs.
