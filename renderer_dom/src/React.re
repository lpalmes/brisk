module DomImplementation = {
  type hostView = Dom.element;

  let instanceMap: Hashtbl.t(int, hostView) = Hashtbl.create(1000);

  let getInstance: int => option(hostView) =
    id =>
      Hashtbl.(mem(instanceMap, id) ? Some(find(instanceMap, id)) : None);

  let memoizeInstance = (id, instance) =>
    Hashtbl.add(instanceMap, id, instance);

  let isDirty = ref(false);
  let markAsDirty = () => isDirty := true;

  let beginChanges = () => ();

  let commitChanges = () => ();

  let mountChild = (~parent: hostView, ~child: hostView, ~position: int) =>
    Webapi.Dom.Element.appendChild(child, parent);

  let unmountChild = (~parent, ~child) =>
    Webapi.Dom.Element.removeChild(child, parent) |> ignore;

  let remountChild = (~parent as _, ~child as _, ~position as _) => ();
};

include ReactCore.Make(DomImplementation);

module RunLoop = {
  let rootRef = ref(None);
  let renderedRef = ref(None);

  let run = (root: DomImplementation.hostView, element: reactElement) => {
    let rendered = RenderedElement.render(element);
    HostView.mountRenderedElement(root, rendered);

    rootRef := Some(root);
    renderedRef := Some(rendered);
  };
};