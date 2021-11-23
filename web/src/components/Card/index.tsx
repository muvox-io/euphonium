import css from "./Card.module.scss";

export default ({ title = "", subtitle = "", children = {} }) => {
  return (
    <div className={css.confWrapper}>
      <div className={css.confWrapper__header}>{title}</div>
      <div className={css.confWrapper__subheader}>{subtitle}</div>
      <div className={css.confWrapper__card}>
        {children}
      </div>
      {/* <div className={css.confWrapper__actionButton}>
        <Button onClick={() => { updateConfiguration() }} />
      </div> */}
    </div>
  );
};
