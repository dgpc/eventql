ZBase.registerView((function() {

  var kServerErrorMsg = "Server Error; please try again and contact support if the problem persists.";

  var finishLogin = function() {
    ZBase.showLoader();
    ZBase.util.httpGet("/a/_/c", function(http) {
      ZBase.hideLoader();
      if (http.status != 200) {
        showErrorMessage(kServerErrorMsg);
        return;
      }

      ZBase.updateConfig(JSON.parse(http.responseText));
      ZBase.navigateTo("/a/");
    });
  };

  var submitForm = function(e) {
    e.preventDefault();

    var postdata = ZBase.util.buildQueryString({
      userid: this.querySelector("input[name='userid']").value,
      password: this.querySelector("input[name='password']").value
    });

    ZBase.showLoader();
    ZBase.util.httpPost("/analytics/api/v1/auth/login", postdata, function(http) {
      ZBase.hideLoader();

      if (http.status == 200) {
        finishLogin();
        return;
      }

      if (http.status == 401) {
        showErrorMessage("Invalid Credentials");
        return;
      }

      showErrorMessage(kServerErrorMsg);
    });

    return false;
  };

  var hideErrorMessage = function() {
    var viewport = document.getElementById("zbase_viewport");
    var elem = viewport.querySelector(".zbase_login .error_message");
    elem.classList.add("hidden");
  };

  var showErrorMessage = function(msg) {
    var viewport = document.getElementById("zbase_viewport");
    var elem = viewport.querySelector(".zbase_login .error_message");
    elem.innerHTML = msg;
    elem.classList.remove("hidden");
  };

  var render = function(path) {
    var conf = ZBase.getConfig();
    if (conf.current_user) {
      ZBase.navigateTo("/a/");
      return;
    }

    var viewport = document.getElementById("zbase_viewport");
    var page = ZBase.getTemplate("login", "zbase_login_form_tpl");

    viewport.innerHTML = "";
    viewport.appendChild(page);

    var form = viewport.querySelector("form");
    form.addEventListener("submit", submitForm);
  };

  return {
    name: "login",
    loadView: function(params) { render(params.path); },
    unloadView: function() {},
    handleNavigationChange: render
  };
})());
