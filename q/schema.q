// classic trade table
// TP stands for ticker-plant compliance, 
// i.e. having sym and time as firt two columns
tradeTP: ([]
    sym:  `symbol$();
    time: `timestamp$();
    price:`float$();
    size: `int$();
    stop: `boolean$();
    ex:   `char$()
    );

// another trade table.
// The time is the last column.
// This can be useful to measure execution time of the K object creation
// of the C api
trade: ([] 
	sym:`symbol$();
	sq:`long$();
	stop:`char$();
	size:`int$();
	price:`float$();
	condition:`char$();
	time:`timestamp$()
	);  