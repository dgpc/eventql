DRAW LINECHART WITH
   XDOMAIN INVERT
   YDOMAIN INVERT
   AXIS BOTTOM
   AXIS LEFT
   LEGEND BOTTOM RIGHT INSIDE;

SELECT city AS series, month AS x, temperature AS y, "circle" as pointstyle
   FROM city_temperatures;
