DRAW LINECHART WITH
   AXIS BOTTOM
   AXIS LEFT
   LEGEND BOTTOM LEFT INSIDE TITLE "Average Temperature"
   YDOMAIN -30.0, 30.0;

SELECT city AS series, month AS x, temperature AS y, "circle" as pointstyle
   FROM city_temperatures;
