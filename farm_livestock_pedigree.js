
// Set default width and height, calculate ratio
var default_width = 960;
var default_height = 695;
var default_ratio = default_width / default_height;

var margin = {top: 0, right: 320, bottom: 0, left: 0},
    width = default_width - margin.left - margin.right,
    height = default_height - margin.top - margin.bottom;
// Determine current size, which determines vars
function set_vars() {
  //alert('setting vars')
  current_width = window.innerWidth - 35;
  current_height = window.innerHeight;

  current_ratio = current_width / current_height;

  // Check if height is limiting factor
  if ( current_ratio > default_ratio ){
    h = current_height;
    w = h * default_ratio;
  // Else width is limiting
  } else {
    w = current_width;
    h = w / default_ratio;
  }

  // Set new width and height based on graph dimensions
  width = w - margin.left - margin.right;
  height = h - margin.top - margin.bottom;

};

set_vars();

function drawGraphic() {
  var tree = d3.layout.tree()
      .separation(function(a, b) { return a.parent === b.parent ? 1 : .5; })
      .children(function(d) { return d.parents; })
      .size([height, width]);

  var svg = d3.select(".farm-pedigree").append("svg")
      .attr("width", width + margin.left + margin.right)
      .attr("height", height + margin.top + margin.bottom)
    .append("g")
      .attr("transform", "translate(" + margin.left + "," + margin.top + ")");

  var json = Drupal.settings.farm_livestock_pedigree.pedigree_data;
  var nodes = tree.nodes(json);

  var link = svg.selectAll(".link")
      .data(tree.links(nodes))
    .enter().append("path")
      .attr("class", "link")
      .attr("d", elbow);

  var node = svg.selectAll(".node")
      .data(nodes)
    .enter().append("g")
      .attr("class", "node")
      .attr("transform", function(d) { return "translate(" + d.y + "," + d.x + ")"; })

  node.append("text")
      .attr("class", "name")
      .attr("x", 8)
      .attr("y", -6)
      .text(function(d) {
        var sex = "";
        if (d.sex == 'M') {
          sex = "♂";
        }
        if (d.sex == 'F') {
          sex = "♀";
        }
        return sex + " " + d.name;
      });

  node.append("text")
      .attr("x", 8)
      .attr("y", 8)
      .attr("dy", ".71em")
      .attr("class", "about type")
      .text(function(d) { return "Breed: " + d.type; });

  node.append("text")
      .attr("x", 8)
      .attr("y", 8)
      .attr("dy", "1.86em")
      .attr("class", "about lifespan")
      .text(function(d) { return "DOB: " + d.dob; });

  function elbow(d, i) {
    return "M" + d.source.y + "," + d.source.x
        + "H" + d.target.y + "V" + d.target.x
        + (d.target.children ? "" : "h" + margin.right);
  }
}
drawGraphic();

// Use a timer so the chart is not constantly redrawn while window is being resized.
var resizeTimer;
window.onresize = function(event) {
 clearTimeout(resizeTimer);
  resizeTimer = setTimeout(function()
  {
    var s = d3.selectAll('.farm-pedigree svg');
    s = s.remove();
    set_vars();
    drawGraphic();
  }, 100);
}
