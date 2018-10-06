open React;

module View = {
  let component = statelessNativeComponent("View");
  let make = children => {
    ...component,
    render: _ => {
      make: () =>
        Webapi.Dom.Document.createElement("div", Webapi.Dom.document),
      shouldReconfigureInstance: (~oldState as _, ~newState as _) => true,
      updateInstance: (_self, node) => (),
      children,
    },
  };
  let createElement = (~children, ()) =>
    element(make(listToElement(children)));
};

module Button = {
  let component = statelessNativeComponent("Button");
  let make = (~title=?, ~callback=() => (), children) => {
    ...component,
    render: _ => {
      make: () => {
        let node =
          Webapi.Dom.Document.createElement("button", Webapi.Dom.document);
        Webapi.Dom.Element.addEventListener("click", _ => callback(), node);
        switch (title) {
        | Some(value) =>
          Webapi.Dom.Document.setTextContent(Obj.magic(node), value)
        | None => ()
        };
        node;
      },
      shouldReconfigureInstance: (~oldState as _, ~newState as _) => true,
      updateInstance: (_self, _) => (),
      children,
    },
  };
  let createElement = (~title=?, ~callback=?, ~children, ()) =>
    element(make(~title?, ~callback?, listToElement(children)));
};